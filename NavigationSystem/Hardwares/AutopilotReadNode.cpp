/**
 * @file    AutopilotReadNode.cpp
 *
 * @brief   Read messages from the Autopilot
 *
 */

#include "AutopilotReadNode.hpp"
#include "../Libs/mavlink/common/mavlink.h"
#include "../MessageBus/MessageTypes.hpp"
#include "../Messages/GPSDataMsg.hpp"
#include "../SystemServices/SysClock.hpp"

AutopilotReadNode::AutopilotReadNode(MessageBus& messageBus, DBHandler& dbhandler, AutopilotInterface& autopilot) :
ActiveNode(NodeID::AutopilotRead, messageBus), m_LoopTime (0.01), m_db(dbhandler), m_autopilot(autopilot)
{
    m_RudderFeedback  = DATA_OUT_OF_RANGE;
    m_WingsailFeedback = DATA_OUT_OF_RANGE;
    m_Radio_Controller_On = DATA_OUT_OF_RANGE;
    m_system_id = 0;
    m_autopilot_id = 0;
    m_companion_id = 0;
    messageBus.registerNode(*this, MessageType::ServerConfigsReceived);
}

AutopilotReadNode::~AutopilotReadNode()
{
}

bool AutopilotReadNode::init()
{
    updateConfigsFromDB();
    m_currentDay = SysClock::day();

    return true;
}

void AutopilotReadNode::updateConfigsFromDB()
{
    m_LoopTime = m_db.retrieveCellAsDouble("config_can_arduino","1","loop_time");
}

void AutopilotReadNode::processMessage (const Message* message)
{
    if(message->messageType() == MessageType::ServerConfigsReceived)
    {
        updateConfigsFromDB();
    }
}

void AutopilotReadNode::start()
{
    m_Running.store(true);
    runThread(AutopilotReadNodeThreadFunc);
}

void AutopilotReadNode::stop()
{
    m_Running.store(false);
    stopThread(this);
}

void AutopilotReadNode::AutopilotReadNodeThreadFunc(ActiveNode* nodePtr)
{
    Logger::info("AutopilotReadNode thread has started");
    AutopilotReadNode* node = dynamic_cast<AutopilotReadNode*> (nodePtr);
    
    bool valid;
    mavlink_message_t message;
    
    Timer timer;
    timer.start();
    
    while(node->m_Running.load() == true)
    {
        // ----------------------------------------------------------------------
        //   READ MESSAGE
        // ----------------------------------------------------------------------
        node->m_lock.lock();
        valid = node->m_autopilot.read_message(message);
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
