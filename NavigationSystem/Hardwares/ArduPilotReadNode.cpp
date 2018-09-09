/**
 * @file    ArduPilotReadNode.cpp
 *
 * @brief   Read messages from the Ardupilot
 *
 */

#include "ArduPilotReadNode.hpp"
#include "../Libs/mavlink/common/mavlink.h"
#include "../MessageBus/MessageTypes.hpp"
#include "../Messages/GPSDataMsg.hpp"
#include "../SystemServices/SysClock.hpp"

#ifdef __APPLE__
char *uart_name = (char*)"/dev/tty.usbmodem1";
#else
char *uart_name = (char*)"/dev/ttyUSB0";
#endif

const int baudrate = 57600;

ArduPilotReadNode::ArduPilotReadNode(MessageBus& messageBus, DBHandler& dbhandler) :
ActiveNode(NodeID::ArduPilotRead, messageBus), m_LoopTime (0.01), m_db(dbhandler)
{
    m_RudderFeedback  = DATA_OUT_OF_RANGE;
    m_WingsailFeedback = DATA_OUT_OF_RANGE;
    m_Radio_Controller_On = DATA_OUT_OF_RANGE;
    m_system_id = 0;
    m_autopilot_id = 0;
    m_companion_id = 0;
    messageBus.registerNode(*this, MessageType::ServerConfigsReceived);
    
}

ArduPilotReadNode::~ArduPilotReadNode(){
    if(m_serial_port->status)
        m_serial_port->close_serial();
}

bool ArduPilotReadNode::init() {
    updateConfigsFromDB();
    
    m_currentDay = SysClock::day();

    Serial_Port serial_port(uart_name, baudrate);
    serial_port.start();
    if ( serial_port.status != 1 ) // SERIAL_PORT_OPEN
    {
        Logger::error("Serial port not open");
        return false;
    }
    
    mavlink_message_t message;
    bool success = false;
    
    // ----------------------------------------------------------------------
    //   HANDLE MESSAGE
    // ----------------------------------------------------------------------
    while ( !success )
    {
        // ----------------------------------------------------------------------
        //   READ MESSAGE
        // ----------------------------------------------------------------------
        mavlink_message_t message;
        success = serial_port.read_message(message);
        
        // ----------------------------------------------------------------------
        //   HANDLE MESSAGE
        // ----------------------------------------------------------------------
        
        // Store message sysid and compid.
        m_system_id  = message.sysid;
        m_companion_id = message.compid;
        
        // Handle Message ID
        if (message.msgid == MAVLINK_MSG_ID_HEARTBEAT)
        {
            Logger::info("Autopilot connection\t\t[OK]");
            success = true;
        }
        usleep(100000);
    }
    serial_port.close_serial();
    
    return true;
}

void ArduPilotReadNode::updateConfigsFromDB()
{
    m_LoopTime = m_db.retrieveCellAsDouble("config_can_arduino","1","loop_time");
}

void ArduPilotReadNode::processMessage (const Message* message){
    if(message->messageType() == MessageType::ServerConfigsReceived)
    {
        updateConfigsFromDB();
    }
}

void ArduPilotReadNode::toggleOffboardControl(bool flag)
{
    // Prepare command for off-board mode
    mavlink_command_long_t com = { 0 };
    com.target_system    = m_system_id;
    com.target_component = m_autopilot_id;
    com.command          = MAV_CMD_NAV_GUIDED_ENABLE;
    com.confirmation     = true;
    com.param1           = (float) flag; // flag >0.5 => start, <0.5 => stop
    
    // Encode
    mavlink_message_t message;
    mavlink_msg_command_long_encode(m_system_id, m_companion_id, &message, &com);
    
    // Send the message
    m_serial_port->write_message(message);
}

void ArduPilotReadNode::start()
{
    m_Running.store(true);
    runThread(ArduPilotReadNodeThreadFunc);
}

void ArduPilotReadNode::stop()
{
    m_Running.store(false);
    stopThread(this);
}

void ArduPilotReadNode::ArduPilotReadNodeThreadFunc(ActiveNode* nodePtr)
{
    Logger::info("ArduPilotNode thread has started");
    
    ArduPilotReadNode* node = dynamic_cast<ArduPilotReadNode*> (nodePtr);
    
    bool valid, received_all;
    mavlink_message_t message;
    Timer timer;
    timer.start();
    Serial_Port serial_port(uart_name, baudrate);
    serial_port.start();
    
    while(node->m_Running.load() == true)
    {
        // ----------------------------------------------------------------------
        //   READ MESSAGE
        // ----------------------------------------------------------------------
        node->m_lock.lock();
        //valid = node->m_serial_port->read_message(message);
        valid = serial_port.read_message(message);
        node->m_lock.unlock();

        // ----------------------------------------------------------------------
        //   HANDLE MESSAGE
        // ----------------------------------------------------------------------
        if( valid )
        {
            // Handle Message ID
            switch (message.msgid)
            {
                case MAVLINK_MSG_ID_MISSION_REQUEST_LIST:
                {
                    Logger::info("MAVLINK_MSG_ID_MISSION_REQUEST_LIST received");
                    mavlink_collision_t collision;
                    mavlink_msg_collision_decode(&message, &collision);
                    
                    break;
                }
                    
                case MAVLINK_MSG_ID_GPS_STATUS:
                {
                    Logger::info("MAVLINK_MSG_ID_GPS_STATUS received");
                    mavlink_gps_raw_int_t gps_status;
                    mavlink_msg_gps_raw_int_decode(&message, &gps_status);
                    
                    // Once a day: make sure system clock is updated from gps to ensure accuracy
                    int today = SysClock::day();
                    double unixTime = static_cast<double>(gps_status.time_usec);
                    if (node->m_currentDay != today)
                    {
                        SysClock::setTime(unixTime);
                        node->m_currentDay = SysClock::day();
                    }
                    
                    // Decode GPS reading
                    double m_Lat;
                    double m_Lon;
                    double m_Speed;
                    double m_Course;
                    
                    double lat = static_cast<double>(gps_status.lat); // North(+) or South(-) [0-90]
                    double lon = static_cast<double>(gps_status.lon); // East(+) or West(-)  [0-180]
                    double speed = static_cast<double>(gps_status.vel); // NOTE : m/s
                    double course = static_cast<double>(gps_status.cog); // NOTE : degree [0-360)
                    int satCount = gps_status.satellites_visible;
                    GPSMode mode = static_cast<GPSMode>(gps_status.fix_type);
                    bool gps_hasFix = gps_status.fix_type;
                    
                    MessagePtr msg = std::make_unique<GPSDataMsg>(gps_hasFix, true, lat, lon, unixTime, speed, course, satCount, mode);
                    node->m_MsgBus.sendMessage(std::move(msg));
                    
                    break;
                }
                    
                case MAVLINK_MSG_ID_SCALED_IMU:
                {
                    Logger::info("MAVLINK_MSG_ID_SCALED_IMU received");
                    mavlink_scaled_imu_t scaled_imu;
                    mavlink_msg_scaled_imu_decode(&message, &scaled_imu);
                    
                    break;
                }
                    
                case MAVLINK_MSG_ID_ATTITUDE:
                {
                    Logger::info("MAVLINK_MSG_ID_ATTITUDE received");
                    mavlink_attitude_t attitude;
                    mavlink_msg_attitude_decode(&message, &attitude);
                    attitude.pitch;
                    attitude.roll;
                }
                    
                case MAVLINK_MSG_ID_BATTERY_STATUS:
                {
                    Logger::info("MAVLINK_MSG_ID_BATTERY_STATUS received");
                    mavlink_battery_status_t battery_status;
                    mavlink_msg_battery_status_decode(&message, &battery_status);
                    break;
                }
            }
        }
        
        timer.sleepUntil(node->m_LoopTime);
        timer.reset();
    }
}
