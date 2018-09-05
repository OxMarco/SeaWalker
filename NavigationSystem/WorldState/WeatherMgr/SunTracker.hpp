//
//  SunTracker.hpp
//  SeaWalker
//
//  Created by Giammarco René Casanova on 05/09/2018.
//  Copyright © 2018 seawalker. All rights reserved.
//

#ifndef SUNTRACKER_HPP
#define SUNTRACKER_HPP

#include <math.h>
#include <time.h>

/**
 * @class SunTracker
 * @brief The class to calculate the sunrise and sunset hours
 */
class SunTracker
{
public:
    SunTracker();
    SunTracker(double, double, int);
    
    void setPosition(double, double, int);
    void setTZOffset(int);
    double setCurrentDate(int, int, int);
    double calcSunriseUTC();
    double calcSunsetUTC();
    double calcSunrise();
    double calcSunset();
    int moonPhase();
    
private:
    double degToRad(double);
    double radToDeg(double);
    double calcMeanObliquityOfEcliptic(double);
    double calcGeomMeanLongSun(double);
    double calcObliquityCorrection(double);
    double calcEccentricityEarthOrbit(double);
    double calcGeomMeanAnomalySun(double);
    double calcEquationOfTime(double);
    double calcTimeJulianCent(double);
    double calcSunTrueLong(double);
    double calcSunApparentLong(double);
    double calcSunDeclination(double);
    double calcHourAngleSunrise(double, double);
    double calcHourAngleSunset(double, double);
    double calcJD(int,int,int);
    double calcJDFromJulianCent(double);
    double calcSunEqOfCenter(double);
    
    double latitude;
    double longitude;
    double julianDate;
    int m_year;
    int m_month;
    int m_day;
    int tzOffset;
    
}; // end of class SunTracker

#endif /* SUNTRACKER_HPP */
