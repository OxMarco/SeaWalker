/**
 * @file   ServerConfigsReceivedMsg.hpp
 *
 * @brief  Notify receivers that new waypoints have been downloaded from the server
 *
 */

#ifndef SERVERWAYPOINTSRECEIVEDMSG_HPP
#define SERVERWAYPOINTSRECEIVEDMSG_HPP

#include "../MessageBus/Message.hpp"

class ServerWaypointsReceivedMsg : public Message {
   public:
    ServerWaypointsReceivedMsg(NodeID destinationID, NodeID sourceID)
        : Message(MessageType::ServerWaypointsReceived, sourceID, destinationID) {}

    ServerWaypointsReceivedMsg()
        : Message(MessageType::ServerWaypointsReceived, NodeID::None, NodeID::None) {}

    ServerWaypointsReceivedMsg(MessageDeserialiser deserialiser) : Message(deserialiser) {}

    virtual ~ServerWaypointsReceivedMsg() {}
};

#endif /* SERVERWAYPOINTSRECEIVEDMSG_HPP */
