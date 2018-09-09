/**
 * @file    AutopilotInterface.hpp
 *
 * @brief   Interface with ArduPilot
 *
 */

#ifndef AUTOPILOTINTERFACE_HPP
#define AUTOPILOTINTERFACE_HPP

#include "../../Network/SerialPort.hpp"
#include "../../Libs/mavlink/common/mavlink.h"
#include <memory>
#include <atomic>
#include <string>

class AutopilotInterface
{
public:
    
    AutopilotInterface();
    ~AutopilotInterface();
    
    void open();
    void close();
    void toggleOffboardControl( bool flag );
    bool read_message(mavlink_message_t&);
    void write_message(mavlink_message_t message);
    
private:
    int m_system_id;
    int m_autopilot_id;
    int m_companion_id;
    int m_currentDay;
    
    std::string m_uart_name;
    int m_baudrate;
    std::shared_ptr<Serial_Port> m_serial_port;
    std::atomic<bool> m_initialised;
};

#endif /* AUTOPILOTINTERFACE_HPP */
