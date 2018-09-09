/**
 * @file        sigtraps.h
 * @version     1.0.0
 * @date        September, 2017
 * @brief       Install signal handlers to handle stack overflow, sigabort and terminal exit correctly.
 *
 */

#ifndef SIGTRAPS_H
#define SIGTRAPS_H

#include "SystemServices/Logger.hpp"
#include <cstdlib>
#include <csignal>

extern "C" void default_handler(int signal)
{
    switch(signal)
    {
        case SIGHUP:
            Logger::info("Caught sighup\n");
            break;
            
        case SIGFPE:
            Logger::info("Caught sigfpe\n");
            exit(EXIT_FAILURE);
            break;
            
        case SIGSEGV:
            Logger::error("Caught segfault\n");
            exit(EXIT_FAILURE);
            break;
    }
}

void install_sig_traps()
{
    /**
     *  @link http://en.cppreference.com/w/cpp/utility/program/SIG_types
     */
    std::signal(SIGHUP, default_handler);
    std::signal(SIGFPE, default_handler);
    std::signal(SIGSEGV, default_handler);
}

#endif /* SIGTRAPS_H */
