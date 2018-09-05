

#include "CANMarineSensorTransmissionNode.h"

#include "../MessageBus/NodeIDs.hpp"
#include "../MessageBus/MessageTypes.hpp"
#include "../SystemServices/Timer.hpp"
#include "CAN_Services/CanBusCommon/CanMessageHandler.h"
#include "CAN_Services/CanBusCommon/canbus_defs.h"

CANMarineSensorTransmissionNode::CANMarineSensorTransmissionNode(MessageBus &msgBus, CANService &canService)
                                                                : Node(NodeID::MarineSensorCANTransmission, msgBus),
                                                                 m_CANService(canService), m_arduinoSensorLoopTime(0),
                                                                 m_takeContinousSensorReadings(false) {

}

CANMarineSensorTransmissionNode::~CANMarineSensorTransmissionNode() {}


bool CANMarineSensorTransmissionNode::init() {
    return true;
}

void CANMarineSensorTransmissionNode::processMessage(const Message* message) {

}
