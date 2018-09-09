/**
 * @file    LocalWebServerNode.cpp
 *
 * @brief   Creates a local web server to interact with the user and provide a basic telemetry
 *
 */

#include "LocalWebServerNode.hpp"
#include "../Messages/LocalConfigChangeMsg.h"
#include "../Messages/LocalWaypointChangeMsg.h"
#include "../Messages/ServerConfigsReceivedMsg.hpp"
#include "../Messages/ServerWaypointsReceivedMsg.hpp"
#include "../SystemServices/Timer.hpp"
#include "../Math/Utility.hpp"
#include "../Libs/cpp-httplib/httplib.h"
#include <atomic>

LocalWebServerNode::LocalWebServerNode(MessageBus& msgBus, DBHandler* dbhandler)
: ActiveNode(NodeID::None, msgBus),
m_LoopTime(0.5),
m_dbHandler(dbhandler) {
    msgBus.registerNode(*this, MessageType::LocalWaypointChange);
    msgBus.registerNode(*this, MessageType::LocalConfigChange);
    msgBus.registerNode(*this, MessageType::ServerConfigsReceived);
}

bool LocalWebServerNode::init()
{
    updateConfigsFromDB();    
    return true;
}

void LocalWebServerNode::start() {
    m_Running.store(true);
    runThread(LocalWebServerNodeThreadFunc);
}

void LocalWebServerNode::stop() {
    m_Running.store(false);
    stopThread(this);
}

void LocalWebServerNode::updateConfigsFromDB() {
    m_LoopTime = m_dbHandler->retrieveCellAsDouble("config_httpsync", "1", "loop_time");
}

void LocalWebServerNode::processMessage(const Message* msgPtr) {
    MessageType msgType = msgPtr->messageType();
    
    switch (msgType) {
        case MessageType::LocalWaypointChange:
            
            break;
        case MessageType::LocalConfigChange:
            break;
        case MessageType::ServerConfigsReceived:
            updateConfigsFromDB();
            break;
        default:
            break;
            // Potentially: on new log data. Currently sending constantly in thread
    }
}

void LocalWebServerNode::LocalWebServerNodeThreadFunc(ActiveNode* nodePtr)
{
    Logger::info("LocalWebServerNode thread has started");

    LocalWebServerNode* node = dynamic_cast<LocalWebServerNode*>(nodePtr);
    
    /*
    httplib::Server svr;
    
    svr.Get("/index", [](const httplib::Request& req, httplib::Response& res) {
        res.set_content("Status page", "text/html");
    });
    
    svr.set_error_handler([](const auto& req, auto& res) {
        res.set_content("404 error - Page not found", "text/plain");

    });
    
    svr.listen("localhost", 8080);
     */
}

std::string LocalWebServerNode::createPage()
{
    std::string html = "<!doctype html><html lang='en'><head><meta charset='utf-8'><title>SeaWalker Status Page</title></head><body>Guess what</body></html>";

    return html;
}

