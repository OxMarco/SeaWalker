/**
 * @file   Logger.hpp
 * @version 1.0.0
 * @author Giammarco Rene' Casanova
 * @date   September, 2017
 * @brief  Logger class
 *
 * @link https://github.com/gabime/spdlog
 *
 */

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <stdexcept>
#include "../Libs/spdlog/spdlog.h"

/**
 *  @class Logger
 *  @brief Logging functionality
 */
class Logger
{
    
public:
    
    ///////////////////// Constructor & Destructor /////////////////////
    
    /// Create an empty Logger object
    Logger();
    
    /// Destroy an existing Logger object
    ~Logger();
    
    ///////////////////// Getter Functions /////////////////////
    
    /// Get the logger status (enabled or disabled)
    static bool getLoggingStatus();
    
    ///////////////////// Other Methods /////////////////////
    
    /// Enable or disable logging
    static void setLoggingStatus(bool flag);
    
    /// Initialise and stop logger
    static void init();
    static void shutdown();
    
    /**
     *   @brief  A globally accessible function to log messages to that works exactly like old C function "printf"
     *     @params message The log message
     */
    static void info(std::string message, ...);
    static void error(std::string message, ...);
    static void warning(std::string message, ...);
    static void critical(std::string message, ...);
    
    ///////////////////// Class Attributes /////////////////////
    
private:
    static std::shared_ptr<spdlog::logger> m_logger; ///< logger instance
    static bool m_initialised; ///< flag to avoid multiple initialisations
    
}; // end of class Logger

#endif // LOGGER_HPP
