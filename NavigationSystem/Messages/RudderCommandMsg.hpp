/**
 * @file   RudderCommandMsg.hpp
 *
 * @brief  Set rudder position
 *
 *
 */

#ifndef RUDDERCOMMANDMSG_HPP
#define RUDDERCOMMANDMSG_HPP

#include "../MessageBus/Message.hpp"

class RudderCommandMsg : public Message {
   public:
    RudderCommandMsg(NodeID sourceID, NodeID destinationID, float rudderAngle)
        : Message(MessageType::RudderCommand, sourceID, destinationID),
          m_RudderAngle(rudderAngle) {}

    RudderCommandMsg(float rudderAngle)
        : Message(MessageType::RudderCommand, NodeID::None, NodeID::None),
          m_RudderAngle(rudderAngle) {}

    RudderCommandMsg(MessageDeserialiser deserialiser) : Message(deserialiser) {
        if (!deserialiser.readFloat(m_RudderAngle)) {
            m_valid = false;
        }
    }

    virtual void Serialise(MessageSerialiser& serialiser) const {
        Message::Serialise(serialiser);
        serialiser.serialise(m_RudderAngle);
    }

    virtual ~RudderCommandMsg() {}
    float rudderAngle() const { return m_RudderAngle; }

   private:
    float m_RudderAngle;  // degree [-30, +30[ in vessel reference frame (clockwise from top view)
};

#endif /* RUDDERCOMMANDMSG_HPP */
