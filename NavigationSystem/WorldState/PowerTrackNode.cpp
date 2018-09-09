/****************************************************************************************
 *
 * File:
 *         PowerTrackNode.h
 *
 * Purpose:
 *      Collects voltage and current data from the actuators, ECUs, and solar
 *      panel to track the power, sends it to the logger.
 *
 *
 * Developer Notes:
 *        The ArduinoData may or may not be necessary.
 *
 ***************************************************************************************/

#include "PowerTrackNode.hpp"
#include "../Messages/PowerTrackMsg.hpp"
#include "../Math/CourseMath.hpp"
#include "../SystemServices/Logger.hpp"
#include "../SystemServices/Timer.hpp"
#include <chrono>
#include <thread>

///----------------------------------------------------------------------------------
PowerTrackNode::PowerTrackNode(MessageBus& msgBus, DBHandler& dbhandler, double loopTime)
: ActiveNode(NodeID::PowerTrack, msgBus),
m_CurrentSensorDataCurrent(0), m_CurrentSensorDataVoltage(0),
m_CurrentSensorDataElement((SensedElement)0),m_PowerBalance(0.f), m_Looptime(loopTime),
m_lastElementRead(-1),m_db(dbhandler)
{
    msgBus.registerNode(*this, MessageType::CurrentSensorData);
}

//----------------------------------------------------------------------------------
PowerTrackNode::~PowerTrackNode(){}

//----------------------------------------------------------------------------------
bool PowerTrackNode::init()
{
    return true;
}

///----------------------------------------------------------------------------------
void PowerTrackNode::start()
{
    runThread(PowerTrackThreadFunc);
}

///----------------------------------------------------------------------------------
void PowerTrackNode::processMessage(const Message* msg)
{
    MessageType type = msg->messageType();
    
    switch(type)
    {
            
        case MessageType::CurrentSensorData:
            processCurrentSensorDataMessage((CurrentSensorDataMsg*) msg);
            break;
            
        default:
            return;
    }
}

///----------------------------------------------------------------------------------
void PowerTrackNode::processCurrentSensorDataMessage(CurrentSensorDataMsg* msg)
{
    m_CurrentSensorDataCurrent = msg->getCurrent();
    m_CurrentSensorDataVoltage = msg->getVoltage();
    m_CurrentSensorDataElement = msg->getSensedElement();
    m_Power = m_CurrentSensorDataVoltage * m_CurrentSensorDataCurrent;
    
    //if statement to ensure data flow is balanced.
    if ( m_CurrentSensorDataElement != m_lastElementRead ) {
        
        switch(m_CurrentSensorDataElement)
        {
            case SOLAR_PANEL:
                m_PowerBalance += m_Power;
                break;
                
            case ENGINE:
                m_PowerBalance -= m_Power;
                break;
                
            default :
                break;
        }
        
        m_lastElementRead = m_CurrentSensorDataElement;
    }
}

///----------------------------------------------------------------------------------
void PowerTrackNode::PowerTrackThreadFunc(ActiveNode* nodePtr)
{
    PowerTrackNode* node = dynamic_cast<PowerTrackNode*> (nodePtr);
    
    // Initial sleep time in order for enough data to be transmitted on the first message
    std::this_thread::sleep_for(std::chrono::milliseconds(INITIAL_SLEEP));
    
    Timer timer;
    timer.start();
    while(true)
    {
        //Regulate the rate at whcih the messages are sent
        timer.sleepUntil(node->m_Looptime);
        
        // For testing only (to be removed soon/or shift to debug mode)
        Logger::info("PowerTrackInfo: %f,%f,%f,%f,%d", (float)node->m_CurrentSensorDataCurrent,
                      (float)node->m_CurrentSensorDataVoltage, (float)node->m_PowerBalance, (float)node->m_Power,
                      (uint8_t)node->m_CurrentSensorDataElement);
        
        timer.reset();
    }
}
