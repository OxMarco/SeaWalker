/**
 * @file    ArduPilotReadNode.hpp
 *
 * @brief   Read messages from the Ardupilot
 *
 */

#ifndef ARDUPILOTREADNODE_HPP
#define ARDUPILOTREADNODE_HPP

#include "../Database/DBHandler.hpp"
#include "../MessageBus/ActiveNode.hpp"
#include "../MessageBus/Message.hpp"
#include "../MessageBus/MessageBus.hpp"
#include "../SystemServices/Timer.hpp"
#include "../Network/SerialPort.hpp"
#include <chrono>
#include <iostream>
#include <mutex>
#include <atomic>
#include <thread>
#include <vector>

class ArduPilotReadNode : public ActiveNode {
public:
    ArduPilotReadNode(MessageBus& messageBus, DBHandler& dbhandler);
    ~ArduPilotReadNode();
    bool init();
    void processMessage(const Message* message);
    void start();
    void stop();
    
private:
    static void ArduPilotReadNodeThreadFunc(ActiveNode* nodePtr);
    void toggleOffboardControl(bool flag);
    
    ///----------------------------------------------------------------------------------
    /// Update values from the database as the loop time of the thread and others parameters
    ///----------------------------------------------------------------------------------
    void updateConfigsFromDB();
    
    float m_RudderFeedback;          // NOTE : degree [-MAX_RUDDER_ANGLE ; MAX_RUDDER_ANGLE]
    float m_WingsailFeedback;        // NOTE : degree [-MAX_WINGSAIL_ANGLE ; MAX_WINGSAIL_ANGLE]
    float m_Radio_Controller_On;  // NOTE : use to define message corresponding to the activity of
    // the RC
    double m_LoopTime;            // in seconds (ex: 0.5 s)
    DBHandler& m_db;
    std::mutex m_lock;
    std::atomic<bool> m_Running;

    Serial_Port *m_serial_port;
    int m_system_id;
    int m_autopilot_id;
    int m_companion_id;
    int m_currentDay;
};

#endif /* ARDUPILOTREADNODE_HPP */

