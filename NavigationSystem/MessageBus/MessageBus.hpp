/**
 * @file    MessageBus.hpp
 *
 * @brief   The message bus manages message distribution to nodes allowing nodes to
 *          communicate with one another.
 *
 * @details     Nodes can only be added before the run function is called currently. This is to
 *              reduce the number of thread locks in place and because once the system has
 *              started its very rare that a node should be registered afterwards on the fly.
 *
 */

#ifndef MESSAGEBUS_HPP
#define MESSAGEBUS_HPP

#include "Message.hpp"
#include "Node.hpp"
#include <atomic>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <vector>

typedef std::unique_ptr<Message> MessagePtr;

class Node;

class MessageBus {
   public:
    ///----------------------------------------------------------------------------------
    /// @brief Default constructor
    ///----------------------------------------------------------------------------------
    MessageBus();

    ///----------------------------------------------------------------------------------
    /// @brief Default destructor
    /// @details We really don't need to do memory cleanup here as this class will only be
    ///           cleaned up when the program ends
    ///----------------------------------------------------------------------------------
    ~MessageBus();

    ///----------------------------------------------------------------------------------
    /// @brief Registers a node onto the message bus allowing it receive direct messages. The
    ///         message bus does not own the node.
    ///
    /// @param node 			Pointer to the node that should be registered.
    ///----------------------------------------------------------------------------------
    bool registerNode(Node& node);

    ///----------------------------------------------------------------------------------
    /// @brief Registers a node onto the message bus allowing it receive direct messages and
    ///         also subscribes it for a particular message type.
    ///
    /// @param node 			Pointer to the node that should be registered.
    /// @param msgType 			The type of message to register for
    ///----------------------------------------------------------------------------------
    bool registerNode(Node& node, MessageType msgType);

    ///----------------------------------------------------------------------------------
    /// @brief Enqueues a message onto the message queue for distribution through the message
    ///         bus.
    ///
    /// @param msg 				Pointer to the message that should be enqeued, this
    ///							passes ownership to the MessageBus.
    ///----------------------------------------------------------------------------------
    void sendMessage(MessagePtr msg);

    ///----------------------------------------------------------------------------------
    /// @brief Begins running the message bus and distributing messages to nodes that have been
    ///         registered. This function won't ever return.
    ///----------------------------------------------------------------------------------
    void run();

    void stop();

   private:
    ///----------------------------------------------------------------------------------
    /// @brief Stores information about a registered node and the message types it is interested
    ///         in.
    ///----------------------------------------------------------------------------------
    struct RegisteredNode {
        RegisteredNode(Node& node) : nodeRef(node) {}

        Node& nodeRef;

        ///------------------------------------------------------------------------------
        /// @brief Returns true if a registered node is interested in a message type.
        ///
        /// @param type 		The message type which is checked for.
        ///------------------------------------------------------------------------------
        bool isInterested(MessageType type) {
            for (auto msgType : interestedList) {
                if (type == msgType) {
                    return true;
                }
            }
            return false;
        }

        ///------------------------------------------------------------------------------
        /// @brief Subscribes the registered node for a particular type of message
        ///
        /// @param type 		The message type to subscribe this registered node to.
        ///------------------------------------------------------------------------------
        void subscribe(MessageType type) {
            // Maintain only one copy of each interested type.
            if (not isInterested(type)) {
                interestedList.push_back(type);
            }
        }

       private:
        std::vector<MessageType> interestedList;
    };

    ///----------------------------------------------------------------------------------
    /// @brief Looks for existing registered node for a given node pointer and returns a pointer
    ///         to it. If the node has not yet been registered, it is then registered and a
    ///         pointer to the new RegisteredNode is returned.
    ///----------------------------------------------------------------------------------
    RegisteredNode* getRegisteredNode(Node& node);

    ///----------------------------------------------------------------------------------
    /// @brief Goes through the back message queue and distributes messages, calling
    ///         Node::processMessage(Message*) on nodes that are interested in any given message.
    ///----------------------------------------------------------------------------------
    void processMessages();

    ///----------------------------------------------------------------------------------
    /// @brief Creates a log file for the messages.
    ///----------------------------------------------------------------------------------
    void startMessageLog();

    ///----------------------------------------------------------------------------------
    /// @brief Logs that a message was received.
    ///----------------------------------------------------------------------------------
    void logMessageReceived(Message* msg);

    ///----------------------------------------------------------------------------------
    /// @brief Logs a message that is being processed.
    ///----------------------------------------------------------------------------------
    void logMessage(Message* msg);

    ///----------------------------------------------------------------------------------
    /// @brief Logs that a node consumed the current message being processed.
    ///----------------------------------------------------------------------------------
    void logMessageConsumer(NodeID id);

    ///----------------------------------------------------------------------------------
    /// @brief Logs that the current message being processed has finished being processed.
    ///----------------------------------------------------------------------------------
    // void logMessageProcessed(Message* msg);

    ///----------------------------------------------------------------------------------
    /// @brief Returns a message time stamp in the format HH:MM:SS:MS, a char buffer of 14
    ///         characters needs to be provided.
    ///----------------------------------------------------------------------------------
    void messageTimeStamp(unsigned long unixTime, char* buffer);

    std::vector<RegisteredNode*> m_RegisteredNodes; ///< A list with the subscribed nodes.
    std::queue<MessagePtr>* m_FrontMessages;    ///< The forward facing message queue which messages are append to.
    std::queue<MessagePtr>* m_BackMessages; ///< The backend message queue which contains messages to distribute.
    std::mutex m_FrontQueueMutex;   ///< Guards the front message queue.
    std::atomic<bool> m_Running;    ///< Active flag

#ifdef LOG_MESSAGES
    std::ofstream* m_LogFile;
#endif
};

#endif /* MESSAGEBUS_HPP */
