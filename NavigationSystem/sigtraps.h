/**
 * @file        sigtraps.h
 * @version     1.0.0
 * @date        September, 2017
 * @brief       Install signal handlers to handle stack overflow, sigabort and terminal exit correctly.
 *
 */

#ifndef SIGTRAPS_H
#define SIGTRAPS_H

#include "../SystemServices/Logger.hpp"
#include <cstdlib>
#include <csignal>

/** @todo implement proper handling */
extern "C" void default_handler(int signal)
{    
    /*
     // Save system status
     do
     {
     i++;
     } while(i < nodeList.size() && nodeList[i].first()->getID() != SYSTEM_MANAGER);
     if(nodeList[i].first()->getID() == SYSTEM_MANAGER) // Could be the SystemMgr was not initialised
     nodeList[i].first()->saveSystemStatus();
     
     // Disable all running threads
     for(i = 0; i < nodeList.size(); i++)
     {
     if( nodeList[i].second() )
     nodeList[i].first()->disable();
     }
     
     // Stop the messagebus and unsuscribe all observers
     messageBus->stop();
     */
    
    switch(signal)
    {
        case SIGHUP:
            Logger::info("Caught sighup\n");
            /** @todo disable server */
            break;
            
        case SIGSEGV:
            //AutoConfig::status = common::SystemStatus::REBOOT;
            //AutoConfig::save();
            
            Logger::info("Caught segfault\n");
            exit(-1);
            break;
            
        case SIGTERM:
            break;
    }
}

void atexit_handler()
{
    
}

void install_sig_traps()
{
    /**
     *  @link http://en.cppreference.com/w/cpp/utility/program/SIG_types
     */
    std::signal(SIGTERM, default_handler);
    std::signal(SIGSEGV, default_handler);
    std::signal(SIGINT, default_handler);
    std::signal(SIGILL, default_handler);
    std::signal(SIGABRT, default_handler);
    std::signal(SIGFPE, default_handler);
    
    /**
     *  @link https://en.cppreference.com/w/cpp/utility/program/atexit
     */
    std::atexit(atexit_handler);
}

#endif /* SIGTRAPS_H */
