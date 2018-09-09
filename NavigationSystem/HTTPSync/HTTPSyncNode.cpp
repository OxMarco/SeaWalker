/**
 * @file    HTTPSyncNode.cpp
 *
 * @brief   Handles retrieval and sending of logs, waypoints and configs between database and server.
 *          Also notifies messagebus when new serverdata arrives.
 *
 */
 
#include "HTTPSyncNode.hpp"
#include "../Messages/LocalConfigChangeMsg.h"
#include "../Messages/LocalWaypointChangeMsg.h"
#include "../Messages/ServerConfigsReceivedMsg.hpp"
#include "../Messages/ServerWaypointsReceivedMsg.hpp"
#include "../SystemServices/Timer.hpp"
#include "../Math/Utility.hpp"
#include "../Libs/cpp-httplib/httplib.h"
#include <atomic>

HTTPSyncNode::HTTPSyncNode(MessageBus& msgBus, DBHandler& dbhandler)
    : ActiveNode(NodeID::HTTPSync, msgBus),
      m_removeLogs(1),
      m_LoopTime(0.5),
      m_dbHandler(dbhandler) {
    msgBus.registerNode(*this, MessageType::LocalWaypointChange);
    msgBus.registerNode(*this, MessageType::LocalConfigChange);
    msgBus.registerNode(*this, MessageType::ServerConfigsReceived);
}

bool HTTPSyncNode::init() {
    m_initialised = false;
    m_reportedConnectError = false;

    m_serverURL = "requestbin.fullcontact.com";//m_dbHandler.retrieveCell("config_httpsync", "1", "srv_addr");
    m_endpoint = "/x32mk1x3";
    m_shipID = m_dbHandler.retrieveCell("config_httpsync", "1", "boat_id");
    m_shipPWD = m_dbHandler.retrieveCell("config_httpsync", "1", "boat_pwd");
    updateConfigsFromDB();

    m_initialised = true;

    return m_initialised;
}

void HTTPSyncNode::start() {
    if (m_initialised) {
        m_Running.store(true);
        runThread(HTTPSyncThread);
    } else {
        Logger::error("%s Cannot start HTTPSYNC thread as the node was not correctly initialised!",
                      __PRETTY_FUNCTION__);
    }
}

void HTTPSyncNode::stop()
{
    m_Running.store(false);
    stopThread(this);
}

void HTTPSyncNode::updateConfigsFromDB() {
    m_removeLogs = m_dbHandler.retrieveCellAsInt("config_httpsync", "1", "remove_logs");
    m_pushOnlyLatestLogs =
        m_dbHandler.retrieveCellAsInt("config_httpsync", "1", "push_only_latest_logs");
    m_LoopTime = m_dbHandler.retrieveCellAsDouble("config_httpsync", "1", "loop_time");
}

void HTTPSyncNode::processMessage(const Message* msgPtr) {
    MessageType msgType = msgPtr->messageType();

    switch (msgType) {
        case MessageType::LocalWaypointChange:
            pushWaypoints();
            break;
        case MessageType::LocalConfigChange:
            pushConfigs();
            break;
        case MessageType::ServerConfigsReceived:
            updateConfigsFromDB();
            break;
        default:
            break;
            // Potentially: on new log data. Currently sending constantly in thread
    }
}

void HTTPSyncNode::HTTPSyncThread(ActiveNode* nodePtr) {
    HTTPSyncNode* node = dynamic_cast<HTTPSyncNode*>(nodePtr);

    Logger::info("HTTPSync thread has started");

    node->pushWaypoints();
    node->pushConfigs();

    Timer timer;
    timer.start();
    while (node->m_Running.load() == true) {
        node->getConfigsFromServer();
        node->getWaypointsFromServer();
        node->pushDatalogs();

        timer.sleepUntil(node->m_LoopTime);
        timer.reset();
    }

    Logger::info("HTTPSync thread has exited");
}

bool HTTPSyncNode::pushDatalogs() {
    std::string response = "";

    if (performCURLCall(m_dbHandler.getLogs(m_pushOnlyLatestLogs), "pushAllLogs", response)) {
        // remove logs after push
        if (m_removeLogs) {
            m_dbHandler.clearLogs();
        }
        return true;
    } else if (!m_reportedConnectError) {
        Logger::warning("%s Could not push logs to server:", __PRETTY_FUNCTION__);
    }
    return false;
}

bool HTTPSyncNode::pushWaypoints() {
    std::string waypointsData = m_dbHandler.getWaypoints();
    if (waypointsData.size() > 0) {
        std::string response;
        if (performCURLCall(waypointsData, "pushWaypoints", response)) {
            Logger::info("Waypoints pushed to server");
            return true;
        } else if (!m_reportedConnectError) {
            Logger::warning("%s Failed to push waypoints to server", __PRETTY_FUNCTION__);
        }
    }
    return false;
}

bool HTTPSyncNode::pushConfigs() {
    std::string response;

    Json o = Json::parse( m_dbHandler.getConfigs() );
    httplib::Params params;
    for (Json::iterator it = o.begin(); it != o.end(); ++it) {
        params.emplace(it.key().c_str(), it.value().dump());
    }
    
    if (performCURLCall(m_dbHandler.getConfigs(), "pushConfigs", response)) {
        Logger::info("Configs pushed to server");
        return true;
    } else if (!m_reportedConnectError) {
        Logger::warning("%s Error: ", __PRETTY_FUNCTION__);
    }

    return false;
}

std::string HTTPSyncNode::getData(std::string call) {
    std::string response = "";

    if (performCURLCall("", call, response)) {
        return response;
    } else {
        return "";
    }
}

bool HTTPSyncNode::checkIfNewConfigs() {
    std::string result = getData("checkIfNewConfigs");
    if (result.length()) {
        return Utility::safe_stoi(result);
    }
    return false;
}

bool HTTPSyncNode::checkIfNewWaypoints() {
    std::string result = getData("checkIfNewWaypoints");
    if (result.length()) {
        return Utility::safe_stoi(result);
    }
    return false;
}

bool HTTPSyncNode::getConfigsFromServer() {
    if (checkIfNewConfigs()) {
        std::string configs = getData("getAllConfigs");
        if (configs.size() > 0) {
            m_dbHandler.updateConfigs(configs);
            if (not m_dbHandler.updateTable("state", "configs_updated", "1", "1")) {
                Logger::error("%s Error updating state table", __PRETTY_FUNCTION__);
                return false;
            }

            MessagePtr newServerConfigs = std::make_unique<ServerConfigsReceivedMsg>();
            m_MsgBus.sendMessage(std::move(newServerConfigs));
            Logger::info("Configuration retrieved from remote server");
            return true;
        } else if (!m_reportedConnectError) {
            Logger::error("%s Error: %s", __PRETTY_FUNCTION__);
        }
    }
    return false;
}

bool HTTPSyncNode::getWaypointsFromServer() {
    if (checkIfNewWaypoints()) {
        std::string waypoints = getData("getWaypoints");
        if (waypoints.size() > 0) {
            if (m_dbHandler.updateWaypoints(waypoints)) {
                // EVENT MESSAGE - REPLACES OLD CALLBACK, CLEAN OUT CALLBACK REMNANTS IN OTHER
                // CLASSES
                MessagePtr newServerWaypoints = std::make_unique<ServerWaypointsReceivedMsg>();
                m_MsgBus.sendMessage(std::move(newServerWaypoints));

                Logger::info("Waypoints retrieved from remote server");
                return true;
            }

        } else if (!m_reportedConnectError) {
            Logger::warning("%s Could not fetch any new waypoints", __PRETTY_FUNCTION__);
        }
    }
    return false;
}

bool HTTPSyncNode::performCURLCall(std::string data, std::string call, std::string& response)
{
    std::string serverCall = "";
    
    if (!data.empty()) {
        serverCall = "serv=" + call + "&id=" + m_shipID + "&pwd=" + m_shipPWD +
                     "&data=" + data;
    } else {
        serverCall = "serv=" + call + "&id=" + m_shipID + "&pwd=" + m_shipPWD;
    }

    httplib::Client cli(m_serverURL.c_str(), 80);
    
    auto res = cli.Post(m_endpoint.c_str(), serverCall, "text/plain");
    if(res)
    {
        // connection was successful
        m_reportedConnectError = false;

        // check return status
        if (res->status == 200)
            Logger::info("Server response %s", res->body.c_str());
        else
            Logger::info("Server error %d", res->status);
    }
    else
    {
        // errors in connection
        m_reportedConnectError = true;
    }
    
    return m_reportedConnectError;
}
