/**
 * @file    Message.hpp
 *
 * @brief   Provides the base message class
 *
 */

#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include "MessageDeserialiser.hpp"
#include "MessageSerialiser.hpp"
#include "MessageTypes.hpp"
#include "NodeIDs.hpp"

#define LOG_MESSAGES
#ifdef LOG_MESSAGES
    #include "../SystemServices/SysClock.hpp"
#endif

const int DATA_OUT_OF_RANGE = -2000;
const float NO_COMMAND = -1000;

class Message {
   public:
    ///----------------------------------------------------------------------------------
    /// @brief Default constructor
    ///
    /// @param msgType 			The type of message this is.
    /// @param msgSource		Which node posted this message.
    /// @param msgDest			Which node the message should go to. Leave this as
    ///							NodeIDS::NONE for no specific destination.
    ///----------------------------------------------------------------------------------
    Message(MessageType msgType, NodeID msgSource, NodeID msgDest)
        : m_valid(true), m_MessageType(msgType), m_SourceID(msgSource), m_DestinationID(msgDest) {}

    ///----------------------------------------------------------------------------------
    /// @brief A message with no specific destination.
    ///
    /// @param msgType 			The type of message this is.
    /// @param msgSource		Which node posted this message.
    ///----------------------------------------------------------------------------------
    Message(MessageType msgType, NodeID msgSource)
        : m_valid(true),
          m_MessageType(msgType),
          m_SourceID(msgSource),
          m_DestinationID(NodeID::None) {}

    ///----------------------------------------------------------------------------------
    /// @brief A message with no specific destination or source.
    ///
    /// @param msgType 			The type of message this is.
    ///----------------------------------------------------------------------------------
    Message(MessageType msgType)
        : m_MessageType(msgType), m_SourceID(NodeID::None), m_DestinationID(NodeID::None) {}

    Message(MessageDeserialiser& deserialiser) : m_valid(true) {
        if (!deserialiser.readMessageType(m_MessageType) || !deserialiser.readNodeID(m_SourceID) ||
            !deserialiser.readNodeID(m_DestinationID)) {
            m_valid = false;
        }
    }

    virtual ~Message() {}

    ///----------------------------------------------------------------------------------
    /// @brief Returns the type of message this is.
    ///
    ///----------------------------------------------------------------------------------
    MessageType messageType() const { return m_MessageType; }

    ///----------------------------------------------------------------------------------
    /// @brief Returns the node which generated this message.
    ///
    ///----------------------------------------------------------------------------------
    NodeID sourceID() const { return m_SourceID; }

    ///----------------------------------------------------------------------------------
    /// @brief Returns the message's destination.
    ///
    ///----------------------------------------------------------------------------------
    NodeID destinationID() const { return m_DestinationID; }

    ///----------------------------------------------------------------------------------
    /// @brief Indicates that the message was constructed correctly
    ///----------------------------------------------------------------------------------
    bool isValid() const { return m_valid; }

    ///----------------------------------------------------------------------------------
    /// @brief Serialises the message into a MessageSerialiser
    ///----------------------------------------------------------------------------------
    virtual void Serialise(MessageSerialiser& serialiser) const {
        serialiser.serialise(m_MessageType);
        serialiser.serialise(m_SourceID);
        serialiser.serialise(m_DestinationID);
    }

#ifdef LOG_MESSAGES
    TimeStamp timeReceived;
#endif

   protected:
    bool m_valid;  ///< Indicates that the message was correctl created

   private:
    MessageType m_MessageType;  ///< The message type
    NodeID m_SourceID;          ///< Which node generated the message
    NodeID m_DestinationID;     ///< The desintation of the message
};

#endif /* MESSAGE_HPP */

