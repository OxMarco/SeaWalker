/**
 * @file    AutopilotWriteNode.cpp
 *
 * @brief   Send messages to the Autopilot
 *
 */

#include "AutopilotWriteNode.hpp"
#include "../Messages/WaypointDataMsg.hpp"
#include "../Messages/RudderCommandMsg.hpp"
#include "../Messages/WingSailCommandMsg.hpp"

///----------------------------------------------------------------------------------
AutopilotWriteNode::AutopilotWriteNode(MessageBus& msgBus, AutopilotInterface& autopilot)
: Node(NodeID::AutopilotWrite, msgBus), m_autopilot(autopilot)
{
    msgBus.registerNode(*this, MessageType::WaypointData);
    msgBus.registerNode(*this, MessageType::WingSailCommand);
    msgBus.registerNode(*this, MessageType::RudderCommand);
}

///----------------------------------------------------------------------------------
AutopilotWriteNode::~AutopilotWriteNode(){}

///----------------------------------------------------------------------------------
bool AutopilotWriteNode::init()
{
    return true;
}

///----------------------------------------------------------------------------------
void AutopilotWriteNode::processMessage(const Message* message)
{
    MessageType type = message->messageType();
    
    switch(type)
    {
        case MessageType::WaypointData:
            //dynamic_cast< const WaypointData*> (message)
            break;
        
        case MessageType::WingSailCommand:
            break;
            
        case MessageType::RudderCommand:
            break;
    }
}
