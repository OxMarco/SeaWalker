/**
 * @file   PowerTrackMsg.hpp
 *
 * @brief  A PowerTrackMsg contains the power state of the vessel at a given time
 *
 */

#ifndef POWERTRACKMSG_HPP
#define POWERTRACKMSG_HPP

#include "../MessageBus/Message.hpp"

class PowerTrackMsg : public Message {
public:
    PowerTrackMsg(NodeID destinationID,
                  NodeID sourceID,
                  float balance)
    : Message(MessageType::PowerTrackMsg, sourceID, destinationID),
    m_PowerBalance(balance) {}
    
    PowerTrackMsg(float balance)
    : Message(MessageType::PowerTrackMsg, NodeID::None, NodeID::None),
    m_PowerBalance(balance) {}
    
    PowerTrackMsg(MessageDeserialiser deserialiser) : Message(deserialiser) {
        if (!deserialiser.readFloat(m_PowerBalance)) {
            m_valid = false;
        }
    }
    
    virtual void Serialise(MessageSerialiser& serialiser) const {
        Message::Serialise(serialiser);
        serialiser.serialise(m_PowerBalance);
    }
    
    virtual ~PowerTrackMsg() {}
    
    float getBalance() const { return m_PowerBalance; }
    
    ///---------------------------------------------------------------------------
    /// @brief Serialises the message into a MessageSerialiser
    ///---------------------------------------------------------------------------
    
private:
    float m_PowerBalance;
};

#endif /* POWERTRACKMSG_HPP */
