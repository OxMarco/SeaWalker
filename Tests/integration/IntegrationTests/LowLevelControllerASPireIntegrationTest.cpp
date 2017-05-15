#include "Nodes/LowLevelControllerNodeASPire.h"
#include "MessageBus/MessageBus.h"
#include "HardwareServices/CAN_Services/CANService.h"
#include "Tests/unit-tests/TestMocks/MockCANReceiver.h"

MessageBus msgBus;

void msgBusLoop() {
    msgBus.run();
}

int main() {
    CANService canService;
    LowLevelControllerNodeASPire node(msgBus, canService);
    std::vector<uint32_t> IDs = { 700 };
    MockCANReceiver mock (canService, IDs);

    auto f = canService.start();
    canService.SetLoopBackMode();
    std::thread t (msgBusLoop);
    t.detach();

    NavigationControlMsg::NavigationState state =
                    NavigationControlMsg::NavigationState::sailToWaypoint;
    msgBus.sendMessage(std::make_unique<NavigationControlMsg>(1.2, 2.3, false, state));
    msgBus.sendMessage(std::make_unique<StateMessage>(1,2,3,4,5));
    msgBus.sendMessage(std::make_unique<WindStateMsg>(6,7,8,9));
    canService.stop();
    f.get();

    std::cout << mock.message_received() << std::endl;

}