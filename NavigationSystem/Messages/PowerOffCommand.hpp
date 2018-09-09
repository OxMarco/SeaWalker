/**
 * @file   PowerOffCommand.hpp
 *
 * @brief  Sends a message to shutdown the system
 *
 */

#ifndef POWEROFFCOMMAND_HPP
#define POWEROFFCOMMAND_HPP

#include "../MessageBus/Message.hpp"

class PowerOffCommand : public Message {
public:
    PowerOffCommand(NodeID sourceID, NodeID destinationID)
    : Message(MessageType::PowerOffCommand, sourceID, destinationID) {}

    PowerOffCommand(NodeID sourceID)
    : Message(MessageType::PowerOffCommand, sourceID) {}

    PowerOffCommand() : Message(MessageType::PowerOffCommand) {}

    PowerOffCommand(MessageDeserialiser deserialiser) : Message(deserialiser) {}
};

#endif /* POWEROFFCOMMAND_HPP */
