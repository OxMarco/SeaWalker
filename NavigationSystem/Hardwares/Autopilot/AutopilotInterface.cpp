/**
 * @file    AutopilotInterface.cpp
 *
 * @brief   Interface with ArduPilot
 *
 */

#include "AutopilotInterface.hpp"
#include "../../SystemServices/Logger.hpp"
#include <stdexcept>

// ----------------------------------------------------------------------------------
//   Autopilot Interface Class
// ----------------------------------------------------------------------------------
#ifdef __APPLE__
#define PORT "/dev/tty.usbmodem1";
#else
#define PORT "/dev/ttyUSB0";
#endif

// ------------------------------------------------------------------------------
AutopilotInterface::
AutopilotInterface()
{
    m_uart_name = PORT;
    m_baudrate = 57600;
    m_initialised.store(false);
}

// ------------------------------------------------------------------------------
void AutopilotInterface::
open()
{
    //Serial_Port serial(m_uart_name.c_str(), m_baudrate);
    m_serial_port = std::make_unique<Serial_Port>(m_uart_name.c_str(), m_baudrate);
    try {
        m_serial_port->open_serial();
        Logger::success("Autopilot init\t\t[OK]");
    }
    catch(std::runtime_error)
    {
        Logger::error("Autopilot init\t\t[FAILED]");
        exit(EXIT_FAILURE);
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
        success = m_serial_port->read_message(message);
        
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
    m_initialised.store(true);
}

// ------------------------------------------------------------------------------
void AutopilotInterface::
close()
{
    if(m_initialised)
        m_serial_port->close_serial();
}

// ------------------------------------------------------------------------------
AutopilotInterface::
~AutopilotInterface()
{}

// ------------------------------------------------------------------------------
void AutopilotInterface::
toggleOffboardControl(bool flag)
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

// ------------------------------------------------------------------------------
bool AutopilotInterface::
read_message(mavlink_message_t& msg)
{
    if(m_initialised)
    {
        return m_serial_port->read_message(msg);
    }
    else
    {
        Logger::error("ArduPilot interface not initialised");
        return false;
    }
}

// ------------------------------------------------------------------------------
void AutopilotInterface::
write_message(mavlink_message_t message)
{
    if(m_initialised)
        m_serial_port->write_message(message);
    else
        Logger::error("ArduPilot interface not initialised");
}
