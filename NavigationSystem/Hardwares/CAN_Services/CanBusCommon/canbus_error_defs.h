/****************************************************************************************
 *
 * File:
 *    canbus_error_defs.h
 *
 * Purpose:
 *    The purpose of this definitions is a unified use of error codes
 *    between Arduino and RPI.
 *
 * Developer Notes:
 *
 ***************************************************************************************/
// clang-format off

#ifndef CANBUS_ERROR_DEFS_H
#define CANBUS_ERROR_DEFS_H


#define NO_ERRORS 0

#define ERROR_SENSOR_PH_NO_CONNECTION 1             // No connection to sensor unit
#define ERROR_SENSOR_PH_SYNTAX 2                    // Syntax error. Happens if we send wrong command to sensor
#define ERROR_SENSOR_PH_NOT_READY 3                 // Not ready. Means we did not wait long enough for sensor to actually get a reading
#define ERROR_SENSOR_PH_NO_DATA 4                   // No data from sensor. Probably sensor failure? Check specifications from sensor documentation

#define ERROR_SENSOR_CONDUCTIVETY_NO_CONNECTION 5   // No connection to sensor unit
#define ERROR_SENSOR_CONDUCTIVETY_SYNTAX 6          // Syntax error. Happens if we send wrong command to sensor
#define ERROR_SENSOR_CONDUCTIVETY_NOT_READY 7       // Not ready. Means we did not wait long enough for sensor to actually get a reading
#define ERROR_SENSOR_CONDUCTIVETY_NO_DATA 8         // No data from sensor. Probably sensor failure? Check specifications from sensor documentation

#define ERROR_SENSOR_TEMPERATURE_NO_CONNECTION 9    // No connection to sensor unit
#define ERROR_SENSOR_TEMPERATURE_SYNTAX 10          // Syntax error. Happens if we send wrong command to sensor
#define ERROR_SENSOR_TEMPERATURE_NOT_READY 11       // Not ready. Means we did not wait long enough for sensor to actually get a reading
#define ERROR_SENSOR_TEMPERATURE_NO_DATA 12         // No data from sensor. Probably sensor failure? Check specifications from sensor documentation

#define ERROR_CANMSG_DATA_OUT_OF_INTERVAL 13        // Can message have data out of interval. Data will be set to 0
#define ERROR_CANMSG_INDEX_OUT_OF_INTERVAL 14       // When overstepping total index of data. Value will be set to 0

#endif
// clang-format on
