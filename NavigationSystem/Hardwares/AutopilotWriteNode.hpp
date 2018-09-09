/**
 * @file    AutopilotWriteNode.hpp
 *
 * @brief   Send messages to the Autopilot
 *
 */

#ifndef AUTOPILOTWRITENODE_HPP
#define AUTOPILOTWRITENODE_HPP

#include "../MessageBus/Node.hpp"
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

class AutopilotWriteNode : public Node {
public:
    AutopilotWriteNode(MessageBus& messageBus, AutopilotInterface& autopilot);
    ~AutopilotWriteNode();
    bool init();
    void processMessage(const Message* message);
    
private:
    void toggleOffboardControl(bool flag);
    
    double m_LoopTime;            // in seconds (ex: 0.5 s)
    std::mutex m_lock;
    std::atomic<bool> m_Running;
    AutopilotInterface &m_autopilot;
};

#endif /* AUTOPILOTWRITENODE_HPP */
