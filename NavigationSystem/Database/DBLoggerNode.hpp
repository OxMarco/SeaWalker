/**
 * @file    DBLoggerNode.hpp
 *
 * @brief   Saves logs to the database.
 *
 */

#ifndef DBLOGGERNODE_HPP
#define DBLOGGERNODE_HPP

#include "DBLogger.hpp"
#include "../MessageBus/ActiveNode.hpp"
#include "../MessageBus/MessageBus.hpp"
#include "../MessageBus/MessageTypes.hpp"
#include <stdint.h>

class DBLoggerNode : public ActiveNode {
   public:
    DBLoggerNode(MessageBus& msgBus, DBHandler& db, int queueSize);

    void processMessage(const Message* message);

    void updateConfigsFromDB();

    void start();

    void stop();

    bool init();

   private:
    static void DBLoggerNodeThreadFunc(ActiveNode* nodePtr);

    DBHandler& m_db;
    DBLogger m_dbLogger;
    
    // REFACTOR
    // struct used from DBHandler.hpp
    LogItem item{
        (double)DATA_OUT_OF_RANGE,         // m_rudderPosition;
        (double)DATA_OUT_OF_RANGE,         // m_wingsailPosition;
        (double)DATA_OUT_OF_RANGE,         // m_engineThrottle
        (bool)false,                       // m_radioControllerOn;
        (double)DATA_OUT_OF_RANGE,         // m_heading;
        (double)DATA_OUT_OF_RANGE,         // m_pitch;
        (double)DATA_OUT_OF_RANGE,         // m_roll;
        (double)DATA_OUT_OF_RANGE,         // m_distanceToWaypoint;
        (double)DATA_OUT_OF_RANGE,         // m_bearingToWaypoint;
        (double)DATA_OUT_OF_RANGE,         // m_courseToSteer;
        (bool)false,                       // m_tack;
        (bool)false,                       // m_goingStarboard;
        (bool)false,                       // m_gpsHasFix;
        (bool)false,                       // m_gpsOnline;
        (double)DATA_OUT_OF_RANGE,         // m_gpsLat;
        (double)DATA_OUT_OF_RANGE,         // m_gpsLon;
        (double)DATA_OUT_OF_RANGE,         // m_gpsUnixTime;
        (double)DATA_OUT_OF_RANGE,         // m_gpsSpeed;
        (double)DATA_OUT_OF_RANGE,         // m_gpsCourse;
        (int)DATA_OUT_OF_RANGE,            // m_gpsSatellite;
        (bool)false,                       // m_routeStarted;
        (float)DATA_OUT_OF_RANGE,          // m_waterTemperature;
        (float)DATA_OUT_OF_RANGE,          // m_outerTemperature;
        (float)DATA_OUT_OF_RANGE,          // m_ambientLight;
        (float)DATA_OUT_OF_RANGE,          // m_pressure;
        (float)DATA_OUT_OF_RANGE,          // m_depth;
        (double)DATA_OUT_OF_RANGE,         // m_vesselHeading;
        (double)DATA_OUT_OF_RANGE,         // m_vesselLat;
        (double)DATA_OUT_OF_RANGE,         // m_vesselLon;
        (double)DATA_OUT_OF_RANGE,         // m_vesselSpeed;
        (double)DATA_OUT_OF_RANGE,         // m_vesselCourse;
        (double)DATA_OUT_OF_RANGE,         // m_trueWindSpeed;
        (double)DATA_OUT_OF_RANGE,         // m_trueWindDir;
        (double)DATA_OUT_OF_RANGE,         // m_apparentWindSpeed;
        (double)DATA_OUT_OF_RANGE,         // m_apparentWindDir;
        (float)DATA_OUT_OF_RANGE,          // m_windSpeed;
        (float)DATA_OUT_OF_RANGE,          // m_windDir;
        (float)DATA_OUT_OF_RANGE,          // m_windTemp;
        (float)DATA_OUT_OF_RANGE,          // m_inHullTemp;
        (float)DATA_OUT_OF_RANGE,          // m_inHullPressure;
        (float)DATA_OUT_OF_RANGE,          // m_inHullHumidity;
        (bool)false,                       // m_leaksDetected;
        (int) DATA_OUT_OF_RANGE,           // m_batteryRemaining;
        (float) DATA_OUT_OF_RANGE,         // m_currentConsumed;
        (float) DATA_OUT_OF_RANGE,         // m_batteryTemperature;
        (float) DATA_OUT_OF_RANGE,         // m_batteryCurrent;
        (float)DATA_OUT_OF_RANGE,          // m_current;
        (float)DATA_OUT_OF_RANGE,          // m_voltage;
        (SensedElement)DATA_OUT_OF_RANGE,  // m_element;
        (std::string) "unknown",           // m_element_str;
        (std::string) "initialized"        // m_timestamp_str;
    };

    double m_loopTime;
    int m_queueSize;

    std::mutex m_lock;
    std::atomic<bool> m_Running;
};

#endif /* DBLOGGERNODE_HPP */

