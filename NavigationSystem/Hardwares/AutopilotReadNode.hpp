/**
 * @file    AutopilotReadNode.hpp
 *
 * @brief   Read messages from the Autopilot
 *
 */

#ifndef AUTOPILOTREADNODE_HPP
#define AUTOPILOTREADNODE_HPP

#include "../Database/DBHandler.hpp"
#include "../MessageBus/ActiveNode.hpp"
#include "../MessageBus/Message.hpp"
#include "../MessageBus/MessageBus.hpp"
#include "../SystemServices/Timer.hpp"
#include "Autopilot/AutopilotInterface.hpp"
#include <chrono>
#include <iostream>
#include <mutex>
#include <atomic>
#include <thread>
#include <vector>

class AutopilotReadNode : public ActiveNode {
public:
    AutopilotReadNode(MessageBus& messageBus, DBHandler& dbhandler, AutopilotInterface& autopilot);
    ~AutopilotReadNode();
    bool init();
    void processMessage(const Message* message);
    void start();
    void stop();
    
private:
    static void AutopilotReadNodeThreadFunc(ActiveNode* nodePtr);
    
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

    AutopilotInterface &m_autopilot;
    int m_system_id;
    int m_autopilot_id;
    int m_companion_id;
    int m_currentDay;
};

#endif /* AUTOPILOTREADNODE_HPP */
