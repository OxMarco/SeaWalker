/**
 * @file        WaveMotion.cpp
 * @version     1.0.0
 * @author      BlueRobotics
 * @date        September, 2017
 * @brief       Definition of WaveMotion class methods
 *
 */

#include "WaveMotion.hpp"
#include "../../SystemServices/SysClock.h"
#include <math.h>

///////////////////// Class Functions Definition /////////////////////

/** @todo check algorithm */

/**
 *   @brief  Returns the average waves height
 *
 *   @return float
 */
float WaveMotion::getHeight()
{
    return m_swellHeight;
}

/**
 *   @brief  Returns the average wave period
 *
 *   @return float
 */
float WaveMotion::getPeriod()
{
    return m_swellPeriod;
}

inline double degrees(double radians)
{
    return radians * (180.0 / M_PI);
}

/**
 *   @brief  Returns the average wave period
 *   @param roll The roll value from the IMU
 *   @param pitch The pitch value from the IMU
 *   @param altitude Altitude got via GPS or barometer
 *   @param accel_z The acceleration value on the Z axis (perpendicular to the water surface)
 *   @link https://github.com/bluerobotics/SolarSurferCore
 *
 *   @return void
 */
void WaveMotion::calculate(float roll, float pitch, float altitude, float accel_z)
{
    float lowPassZAccel;
    
    uint32_t lastCalculate;
    uint32_t lastSwellPeriod;
    uint32_t lastZeroCrossing;
    bool positiveAccel;
    uint16_t zeroCrossingCount;
    
    float maxGPSHeight;
    float minGPSHeight;
    uint32_t lastSwellHeight;
    
    float maxRollPitch;
    uint32_t lastRollPitch;
    
    float dt = (SysClock::millis()-lastCalculate)/1000.0f;
    lastCalculate = SysClock::millis();
    
    // First, calculate the zero crossings of z-accel
    static const float accelTau = 6; // seconds
    float alpha = dt/(accelTau + dt);
    
    lowPassZAccel = lowPassZAccel * (1-alpha) + alpha * accel_z;
    
    if ( SysClock::millis() - lastZeroCrossing > 2000 )
    {
        if ( positiveAccel && lowPassZAccel < 0 )
        {
            positiveAccel = false;
            zeroCrossingCount++;
            lastZeroCrossing = SysClock::millis();
        } else if ( !positiveAccel && lowPassZAccel > 0 )
        {
            positiveAccel = true;
            zeroCrossingCount++;
            lastZeroCrossing = SysClock::millis();
        }
    }
    
    if ( zeroCrossingCount >= 10 )
    {
        m_swellPeriod = (SysClock::millis() - lastSwellPeriod) / 1000.0f / zeroCrossingCount*2;
        lastSwellPeriod = SysClock::millis();
    }
    
    // Calculate swell height
    
    // reset limits in case of greater resolution
    if ( altitude > m_maxGPSHeight ) {
        //   m_maxGPSHeight = Gps::altitude();
    }
    else if ( altitude < minGPSHeight )
    {
        m_minGPSHeight = minGPSHeight;
    }
    
    if ( SysClock::millis() - lastSwellHeight > 360000 )
    {
        lastSwellHeight = SysClock::millis();
        m_swellHeight = m_swellHeight * (1-heightAlpha) + (maxGPSHeight-minGPSHeight) * heightAlpha;
        
        // Reset heights
        m_maxGPSHeight = -10000;
        m_minGPSHeight = 10000;
    }
    
    // Calculate max roll/pitch
    float tilt = degrees( acos( cos(roll) * cos(pitch) ) );
    
    if ( tilt > maxRollPitch )
    {
        maxRollPitch = tilt;
    }
    
    if ( SysClock::millis() - lastRollPitch > 120000 )
    {
        lastRollPitch = SysClock::millis();
        m_rollPitchRange = m_rollPitchRange*(1-rollPitchAlpha) + maxRollPitch * rollPitchAlpha;
        // Reset angle
        maxRollPitch = 0;
    }
}
