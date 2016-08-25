/****************************************************************************************
 *
 * File:
 * 		VesselStateNode.h
 *
 * Purpose:
 *		Maintains the "current" state of the vessel. Collects data from sensor messages
 *		and then resends a collected copy of that data back out for further processing.
 *
 * Developer Notes:
 *
 *
 ***************************************************************************************/

 #pragma once

#include "ActiveNode.h"
#include "Messages/CompassDataMsg.h"
#include "Messages/GPSDataMsg.h"
#include "Messages/WindDataMsg.h"
#include "Messages/ArduinoDataMsg.h"


class VesselStateNode : public ActiveNode {
public:
	VesselStateNode(MessageBus& msgBus);

	///----------------------------------------------------------------------------------
	/// Just returns true, there is nothing to initialise or setup
	///----------------------------------------------------------------------------------
	bool init() { return true; }

	///----------------------------------------------------------------------------------
 	/// Starts the VesselStateNode's thread that pumps out VesselStateMsg's
 	///----------------------------------------------------------------------------------
	void start();

	void processMessage(const Message* msg);

	///----------------------------------------------------------------------------------
	/// Stores compass data from a CompassDataMsg.
	///----------------------------------------------------------------------------------
	void processCompassMessage(CompassDataMsg* msg);

	///----------------------------------------------------------------------------------
	/// Stores the GPS data from a GPSDataMsg.
	///----------------------------------------------------------------------------------
	void processGPSMessage(GPSDataMsg* msg);

	///----------------------------------------------------------------------------------
	/// Stores the wind data from a WindDataMsg.
	///----------------------------------------------------------------------------------
	void processWindMessage(WindDataMsg* msg);
	void processArduinoMessage(ArduinoDataMsg* msg);

private:
	///----------------------------------------------------------------------------------
	/// Starts the VesselStateNode's thread that pumps out VesselStateMsg which contains
	/// data collected from the vessel's sensors
	///----------------------------------------------------------------------------------
	static void VesselStateThreadFunc(void* nodePtr);

	int 	m_CompassHeading;
	int 	m_CompassPitch;
	int 	m_CompassRoll;
	bool	m_GPSHasFix;
	bool	m_GPSOnline;
	double	m_GPSLat;
	double	m_GPSLon;
	double	m_GPSUnixTime;
	double	m_GPSSpeed;
	double	m_GPSHeading;
	int		m_GPSSatellite;
	float	m_WindDir;
	float	m_WindSpeed;
	float 	m_WindTemp;
	int 	m_ArduinoPressure;
	int 	m_ArduinoRudder;
	int 	m_ArduinoSheet;
    int 	m_ArduinoBattery;
	int		m_ArduinoRC;
};