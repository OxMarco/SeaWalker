/**
 * @file    Node.hpp
 *
 * @brief   The base passive node
 *
 */

#ifndef NODE_HPP
#define NODE_HPP

class MessageBus;

#include "Message.hpp"
#include "MessageBus.hpp"
#include "NodeIDs.hpp"
#include <string>

const int INITIAL_SLEEP = 2000;  // milliseconds

class Node {
   public:
    Node(NodeID id, MessageBus& msgBus) : m_MsgBus(msgBus), m_NodeID(id) {}

    virtual ~Node(){};

    ///----------------------------------------------------------------------------------
    /// @brief This function should attempt to initialise or setup any resources it may need to
    ///         function. If this was successful this function should return true.
    ///----------------------------------------------------------------------------------
    virtual bool init() = 0;

    ///----------------------------------------------------------------------------------
    /// @brief Called by the MessageBus when it has a message the node might be interested in.
    ///         A node should register for messages it wants to receive using
    ///         MessageBus::registerNode(Node*, MessageType)
    ///----------------------------------------------------------------------------------
    virtual void processMessage(const Message* message) = 0;

    ///----------------------------------------------------------------------------------
    /// @brief This function should get the last configuring values from the Database
    ///         That enables the user to change this values by the web interface
    ///         which changes the behaviour of the Node and thus the sailing robot behaviour.
    ///----------------------------------------------------------------------------------
    virtual void updateConfigsFromDB(){};
    
    virtual bool isActiveNode() { return false; }

    NodeID nodeID() { return m_NodeID; }
    
    std::string nodeName() { return nodeToString(m_NodeID); }

   protected:
    MessageBus& m_MsgBus;

   private:
    NodeID m_NodeID;
};

#endif /* NODE_HPP */

