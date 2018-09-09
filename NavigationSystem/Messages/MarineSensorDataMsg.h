/****************************************************************************************
 *
 * File:
 *         MarineSensorDataMsg.h
 *
 * Purpose:
 *        A MarineSensorDataMsg contains MarineSensor data such as temperature, conductivity and ph.
 *
 * Developer Notes:
 *
 *
 ***************************************************************************************/

#pragma once

#include "../MessageBus/Message.hpp"

class MarineSensorDataMsg : public Message {
public:
    MarineSensorDataMsg(NodeID destinationID,
                        NodeID sourceID,
                        float temperature,
                        float conductivity,
                        float ph,
                        float salinity)
    : Message(MessageType::MarineSensorData, sourceID, destinationID),
    m_temperature(temperature),
    m_conductivity(conductivity),
    m_ph(ph),
    m_salinity(salinity) {}
    
    MarineSensorDataMsg(float temperature, float conductivity, float ph, float salinity)
    : Message(MessageType::MarineSensorData, NodeID::None, NodeID::None),
    m_temperature(temperature),
    m_conductivity(conductivity),
    m_ph(ph),
    m_salinity(salinity) {}
    
    MarineSensorDataMsg(MessageDeserialiser& deserialiser) : Message(deserialiser) {
        if (!deserialiser.readFloat(m_temperature) || !deserialiser.readFloat(m_conductivity) ||
            !deserialiser.readFloat(m_ph) || !deserialiser.readFloat(m_salinity)) {
            m_valid = false;
        }
    }
    
    virtual ~MarineSensorDataMsg() {}
    
    float temperature() const { return m_temperature; }
    float conductivity() const { return m_conductivity; }
    float ph() const { return m_ph; }
    float salinity() const { return m_salinity; }
    
    ///----------------------------------------------------------------------------------
    /// Serialises the message into a MessageSerialiser
    ///----------------------------------------------------------------------------------
    virtual void Serialise(MessageSerialiser& serialiser) const {
        Message::Serialise(serialiser);
        
        serialiser.serialise(m_temperature);
        serialiser.serialise(m_conductivity);
        serialiser.serialise(m_ph);
        serialiser.serialise(m_salinity);
    }
    
private:
    float m_temperature;
    float m_conductivity;
    float m_ph;
    float m_salinity;
};
