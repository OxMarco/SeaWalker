/**
 * @file    ActiveNode.hpp
 *
 * @brief   An active node is a base(passive) node that has a thread running.
 *
 */

#ifndef ACTIVENODE_HPP
#define ACTIVENODE_HPP

#include "Node.hpp"
#include <thread>

class ActiveNode : public Node {
   public:
    ActiveNode(NodeID id, MessageBus& msgBus) : Node(id, msgBus) {}

    ///----------------------------------------------------------------------------------
    /// This function should be used to start the active nodes thread.
    ///
    ///----------------------------------------------------------------------------------
    virtual void start() = 0;

   protected:
    void runThread(void (*func)(ActiveNode*));
    void stopThread(ActiveNode* node);

   private:
    //std::thread* m_Thread;
    std::unique_ptr<std::thread> m_Thread;
};

#endif /* ACTIVENODE_HPP */
