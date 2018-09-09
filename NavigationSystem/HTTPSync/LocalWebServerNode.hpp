/**
 * @file    LocalWebServerNode.hpp
 *
 * @brief   Creates a local web server to interact with the user and provide a basic telemetry
 *
 */

#ifndef LOCALWEBSERVERNODE_HPP
#define LOCALWEBSERVERNODE_HPP

#include "../Database/DBHandler.hpp"
#include "../MessageBus/ActiveNode.hpp"
#include "../SystemServices/Logger.hpp"

#include <atomic>
#include <chrono>
#include <string>
#include <thread>

class LocalWebServerNode : public ActiveNode {
public:
    LocalWebServerNode(MessageBus& msgBus, DBHandler* dbhandler);
    
    virtual ~LocalWebServerNode() {}
    
    ///----------------------------------------------------------------------------------
    /// Retrieves server settings from database and initialises curl
    ///
    ///----------------------------------------------------------------------------------
    bool init();
    void start();
    void stop();
    
    ///----------------------------------------------------------------------------------
    /// Pushes waypoints or configurations on new local changes
    ///----------------------------------------------------------------------------------
    void processMessage(const Message* message);
    ///----------------------------------------------------------------------------------
    /// Push functions: sends local data to server using curl
    ///----------------------------------------------------------------------------------
    
private:
    ///----------------------------------------------------------------------------------
    /// Sends server request in curl format - used for all syncing functionality
    ///----------------------------------------------------------------------------------
    bool performCURLCall(std::string data, std::string call, std::string& response);
    
    bool checkIfNewConfigs();
    bool checkIfNewWaypoints();
    
    ///----------------------------------------------------------------------------------
    /// Node thread: Calls all syncing functions while running
    ///----------------------------------------------------------------------------------
    static void LocalWebServerNodeThreadFunc(ActiveNode* nodePtr);
    
    void updateConfigsFromDB();
    
    std::string createPage();
    
    ///----------------------------------------------------------------------------------
    /// Convenience function: creates curl call from argument and returns response (json data)
    ///----------------------------------------------------------------------------------
    std::string getData(std::string call);
    
    ///----------------------------------------------------------------------------------
    /// Determines whether or not to clear all local logs after a successful push to server
    ///----------------------------------------------------------------------------------
    double m_LoopTime;  // units : seconds (ex : 0.5 s)
    
    std::atomic<bool> m_Running;
    DBHandler* m_dbHandler;
};

#endif /* LOCALWEBSERVERNODE_HPP */

