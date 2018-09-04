/**
 * @file   Timer.cpp
 * @version 1.0.0
 * @author SailingRobots team
 * @date   2017
 * @brief  Timer class
 *
 * @details Definition of Time class members
 */

#include <thread>
#include "Timer.h"

///////////////////// Class Functions Definition /////////////////////

#define STEADY_CLOCK std::chrono::steady_clock
#define DURATION_CAST std::chrono::duration_cast
#define DURATION std::chrono::duration

/**
 *   @brief  Timer object constructor
 *   @detail Initialises the starting time, nulls the timePassed counter and sets the running flag to 0
 */
Timer::Timer()
{
    m_start = STEADY_CLOCK::now();
    m_running = false;
    m_timePassed = 0;
}

/**
 *   @brief  Starts the timer
 *   @detail Initialises the starting time and sets the running flag to 1
 */
void Timer::start()
{
    if (!m_running)
    {
        m_start = STEADY_CLOCK::now();
        m_running = true;
    }
}

/**
 *   @brief  Resets the timer
 *   @detail Reinitialises the starting time and sets the running flag to 1
 */
void Timer::reset()
{
    m_start = STEADY_CLOCK::now();
    m_running = true;
}

/**
 *   @brief  Stops the timer
 *   @detail Computes the time passed and sets the running flag to 0
 */
void Timer::stop()
{
    m_timePassed = timePassed();
    m_running = false;
}

/**
 *   @brief  Calculates the time passed
 *
 *   @return double
 */
double Timer::timePassed()
{
    if(m_running)
    {
        STEADY_CLOCK::time_point end;
        DURATION<double> time_span;
        
        // the time to count the difference is not included
        end = STEADY_CLOCK::now();
        time_span = DURATION_CAST<DURATION<double>>(end - m_start);
        return time_span.count();
    }
    else
    {
        return m_timePassed;
    }
}

/**
 *   @brief  Calculates the time remaining
 *
 *   @return double
 */
double Timer::timeUntil(double unixTime)
{
    return unixTime - timePassed();
}

/**
 *   @brief  Suspends the Timer instance
 */
void Timer::sleepUntil(double seconds)
{
    long int microseconds;
    int toMicro = 1000*1000; // microseconds (us) = 10⁻⁶ s
    
    microseconds = timeUntil(seconds) * toMicro;
    std::this_thread::sleep_for(std::chrono::microseconds(microseconds));
}

/**
 *   @brief  Checks if the timer has reached its point
 *
 *   @return bool
 */
bool Timer::timeReached(double seconds)
{
    if (m_running && timePassed() > seconds)
    {
        return true;
    }
    return false;
}
