/**
 * @file        WaveMotion.hpp
 * @version     1.0.0
 * @author      BlueRobotics
 * @date        September, 2017
 * @brief       Class to calculate waves height and speed using IMU and GPS data
 *
 */

#ifndef WAVEMOTION_HPP
#define WAVEMOTION_HPP

/**
 *  @class WaveMotion
 */
class WaveMotion
{
    
public:
    
    ///////////////////// Constructor & Destructor /////////////////////
    
    /// Create an empty WaveMotion object
    WaveMotion() {};
    
    /// Destroy an existent WaveMotion object
    ~WaveMotion() {};
    
    ///////////////////// Getter Functions /////////////////////
    
    /// Get wave height
    float getHeight();
    
    /// Get wave motion period
    float getPeriod();
    
    ///////////////////// Other Methods /////////////////////
    
    /// Update the values of wave motion data
    void calculate(float roll, float pitch, float altitude, float accel_z);
    
    ///////////////////// Class Attributes /////////////////////
    
private:
    const float heightAlpha = 0.375; // 10 minute tau
    const float rollPitchAlpha = 0.71; // About 5 minutes
    
    float m_minGPSHeight = 10000.0f; ///< the minimum altitude value supported by the GPS sensor resolution
    float m_maxGPSHeight = -10000.0f;  ///< the maximum altitude value supported by the GPS sensor resolution
    
    float m_rollPitchRange; ///< the variation range of roll and pitch values
    float m_swellHeight; ///< the wave average heigth
    float m_swellPeriod; ///< the wave average period
    
}; // end of class WaveMotion

#endif  // WAVEMOTION_HPP
