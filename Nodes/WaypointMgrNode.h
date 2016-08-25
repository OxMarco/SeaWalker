/****************************************************************************************
 *
 * File:
 * 		WaypointNode.h
 *
 * Purpose:
 *		The WaypointNode sends information about the waypoints to the sailing logic
 *
 * Developer Notes:
 *
 *
 ***************************************************************************************/

#pragma once


#include "Node.h"
#include "Messages/GPSDataMsg.h"
#include "dbhandler/DBHandler.h"
#include "utility/CourseMath.h"
#include "utility/Timer.h"


class WaypointMgrNode : public Node {
public:
	WaypointMgrNode(MessageBus& msgBus, DBHandler& db);
    virtual ~WaypointMgrNode(){};

	bool init();

	void processMessage(const Message* message);

private:
	void processGPSMessage(GPSDataMsg* msg);
    bool waypointReached();

	///----------------------------------------------------------------------------------
 	/// Sends message with data about the next waypoint
 	///----------------------------------------------------------------------------------
    void sendMessage();
    bool harvestWaypoint();

    DBHandler &m_db;
    bool writeTime;

    int     m_nextId;
    double  m_nextLongitude;
    double  m_nextLatitude;
    int     m_nextDeclination;
    int     m_nextRadius;
    int     m_nextStayTime;

    int     m_prevId;
    double  m_prevLongitude;
    double  m_prevLatitude;
    int     m_prevDeclination;
    int     m_prevRadius;

    double  m_gpsLongitude;
    double  m_gpsLatitude;

    Timer   m_waypointTimer;
};