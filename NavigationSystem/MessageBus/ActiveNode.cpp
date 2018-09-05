/**
 * @file    ActiveNode.cpp
 *
 * @brief   An active node is a base(passive) node that has a thread running.
 *
 */

#include "ActiveNode.hpp"
#include  <stdexcept>

void ActiveNode::runThread(void(*func)(ActiveNode*))
{
    m_Thread.reset(new std::thread(func, this));
    
    if(m_Thread == nullptr)
        throw std::runtime_error("impossible to start the node thread");
}

void ActiveNode::stopThread(ActiveNode* node)
{
    if( node->m_Thread != nullptr && node->m_Thread->joinable() )
        node->m_Thread->join();
    
    node->m_Thread.reset();
}
