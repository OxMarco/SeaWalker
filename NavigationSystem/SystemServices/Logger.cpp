/**
 * @file   Logger.cpp
 * @version 1.0.0
 * @author Giammarco Rene' Casanova
 * @date   September, 2017
 * @brief  Definition of Logger class methods
 */

#include "Logger.hpp"

namespace spd = spdlog;

// Logger config params
#define DEFAULT_LOG_NAME    "boat.log"
#define LOGFILE_PATH        "~/logs"
#define MAX_LOGFILE_SIZE    1048576 * 5 // 5 Mb
#define LOGFILES_N          3
#define QUEUE_SIZE          4096 // Must be a power of 2

std::shared_ptr<spdlog::logger> Logger::m_logger;
bool Logger::m_initialised;

/**
 * @brief Initialises the singleton logger system, returns false if it is unable to generate a log file
 *
 */
void Logger::init()
{
    try
    {
        // Full queue policy: Discard the message and never block
        spd::set_async_mode(QUEUE_SIZE, spd::async_overflow_policy::discard_log_msg);
        
        // Set the logger as rotating and asyncronous
        //m_logger = spd::rotating_logger_mt(DEFAULT_LOG_NAME, LOGFILE_PATH, MAX_LOGFILE_SIZE, LOGFILES_N);
        m_logger = spdlog::stdout_color_mt("console");
        spd::set_pattern("*** [%H:%M:%S %z] [thread %t] %v ***");
        
        // Initialisation was successful
        m_initialised = true;
    }
    catch (const spd::spdlog_ex& ex)
    {
        m_initialised = false;
    }
}

/**
 * @brief default destructor
 *
 */
Logger::~Logger()
{
    if(m_initialised)
        shutdown();
}

/**
 * @brief Delete logger object and close all handles
 *
 */
void Logger::shutdown()
{
    try
    {
        m_initialised = false;
        spd::drop_all();
    }
    catch (const spd::spdlog_ex& ex)
    {
    }
}

/**
 * @brief Get logging status (if initialised)
 *
 * @return bool
 */
bool Logger::getLoggingStatus()
{
    return m_initialised;
}

/// Logging types

void Logger::info(std::string message, ...)
{
    if(!m_initialised) { return; }
    
    // Avoid logging empty strings
    if(message.empty())
        return;
    
    va_list args;
    char logBuffer[MAX_LOGFILE_SIZE];
    // Put together the formatted string
    va_start(args, message);
    vsnprintf(logBuffer, MAX_LOGFILE_SIZE, message.c_str(), args);
    va_end(args);
    
    m_logger->info(logBuffer);
}

void Logger::warning(std::string message, ...)
{
    if(!m_initialised) { return; }
    
    // Avoid logging empty strings
    if(message.empty())
        return;
    
    va_list args;
    char logBuffer[MAX_LOGFILE_SIZE];
    // Put together the formatted string
    va_start(args, message);
    vsnprintf(logBuffer, MAX_LOGFILE_SIZE, message.c_str(), args);
    va_end(args);
    
    m_logger->warn(logBuffer);
}

void Logger::error(std::string message, ...)
{
    if(!m_initialised) { return; }
    
    // Avoid logging empty strings
    if(message.empty())
        return;
    
    va_list args;
    char logBuffer[MAX_LOGFILE_SIZE];
    // Put together the formatted string
    va_start(args, message);
    vsnprintf(logBuffer, MAX_LOGFILE_SIZE, message.c_str(), args);
    va_end(args);
    
    m_logger->error(logBuffer);
}

void Logger::critical(std::string message, ...)
{
    if(!m_initialised) { return; }
    
    // Avoid logging empty strings
    if(message.empty())
        return;
    
    va_list args;
    char logBuffer[MAX_LOGFILE_SIZE];
    // Put together the formatted string
    va_start(args, message);
    vsnprintf(logBuffer, MAX_LOGFILE_SIZE, message.c_str(), args);
    va_end(args);
    
    m_logger->critical(logBuffer);
}
