/*
 * global.h
 *
 *  Created on: May 20, 2015
 *      Author: sailbot
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <memory>
#include <thread>
#include "SailingRobot.h"
#include "xBeeSync.h"
#include "GPSupdater.h"
#include "WindsensorController.h"

SailingRobot *sr_handle;
std::unique_ptr<xBeeSync> xbee_handle;
GPSupdater *gps_handle;
std::unique_ptr<WindsensorController> windsensor_handle;

std::thread windsensor_thread;

bool m_mockGPS;
bool m_xBeeOFF;
bool m_mockWindsensor;

#endif /* MAIN_H_ */
