/**
 * @file   CourseMath.hpp
 * 
 * @brief  Course calculation helpers
 *
 */

#ifndef COURSEMATH_HPP
#define COURSEMATH_HPP

#include <stdint.h>

class CourseMath {
   public:
    static int16_t calculateBTW(double gpsLon,
                                double gpsLat,
                                double waypointLon,
                                double waypointLat);
    static double calculateDTW(double gpsLon,
                               double gpsLat,
                               double waypointLon,
                               double waypointLat);

   private:
    CourseMath(){};
};

#endif /* COURSEMATH_HPP */
