/**
 * @file    main.cpp
 *
 * @brief   The entry point for the control system
 *
 */

#include "Database/DBHandler.hpp"
#include "Database/DBLoggerNode.hpp"

#include "HTTPSync/HTTPSyncNode.hpp"

#include "MessageBus/MessageBus.hpp"
#include "Messages/DataRequestMsg.h"

#include "SystemServices/Logger.hpp"

#include "WorldState/StateEstimationNode.h"
#include "WorldState/WindStateNode.h"
#include "WorldState/CollidableMgr/CollidableMgr.h"

#include "LowLevelControllers/WingSailControlNode.hpp"
#include "LowLevelControllers/CourseRegulatorNode.hpp"

#include "Navigation/WaypointMgrNode.hpp"

#if LOCAL_NAVIGATION_MODULE == 1
  #include "Navigation/LocalNavigationModule/LocalNavigationModule.h"
  #include "Navigation/LocalNavigationModule/Voters/WaypointVoter.h"
  #include "Navigation/LocalNavigationModule/Voters/WindVoter.h"
  #include "Navigation/LocalNavigationModule/Voters/ChannelVoter.h"
  #include "Navigation/LocalNavigationModule/Voters/ProximityVoter.h"
  #include "Navigation/LocalNavigationModule/Voters/MidRangeVoter.h"
#else
  #include "Navigation/LineFollowNode.hpp"
#endif

#if SIMULATION == 1
  #include "Simulation/SimulationNode.h"
#else
  #include "Hardwares/HMC6343Node.h"
  #include "Hardwares/GPSDNode.h"
  #include "Hardwares/CAN_Services/CANService.h"
  #include "Hardwares/CANWindsensorNode.h"
  #include "Hardwares/ActuatorNodeASPire.h"
  #include "Hardwares/CANArduinoNode.h"

    #include "Hardwares/CANMarineSensorReceiver.h"
    #include "Hardwares/CANMarineSensorTransmissionNode.h"
    #include "Hardwares/CANCurrentSensorNode.h"

#endif

#include "sigtraps.h"

#include <string>
#include <iostream>
#include <map>

enum class NodeImportance {
	CRITICAL,
	NOT_CRITICAL
};

//std::map<Node*,std::string> nodeList; // store each node and whether was correctly initialised or not

///----------------------------------------------------------------------------------
/// @brief Initialises a node and shutsdown the program if a critical node fails.
///
/// @param node 			A pointer to the node to initialise
/// @param nodeName 		A string name of the node, for logging purposes.
/// @param importance 		Whether the node is a critcal node or not critical. If a
///							critical node fails to initialise the program will
///							shutdown.
///
///----------------------------------------------------------------------------------
void initialiseNode(Node& node, const char* nodeName, NodeImportance importance)
{
	if(node.init())
	{
		Logger::info("Node: %s - init\t[OK]", nodeName);
        //nodeList.insert(std::pair<Node*,std::string>(node, nodeName));
	}
	else
	{
		Logger::error("Node: %s - init\t\t[FAILED]", nodeName);

		if(importance == NodeImportance::CRITICAL)
		{
			Logger::error("Critical node failed to initialise, shutting down");
			exit(1);
		}
	}
}

void atexit_handler()
{
    // Shutdown
    //-------------------------------------------------------------------------------
    std::cout<<std::endl<<"...Exiting..."<<std::endl;
    
    /*for (std::map<Node*,std::string>::iterator it=nodeList.begin(); it!=nodeList.end(); ++it)
     {
     it->first.stopThread();
     Logger::info("Node: %s - stopped\t[OK]", it->second);
     }*/
}

///----------------------------------------------------------------------------------
/// Entry point, can accept one argument containing a relative path to the database.
///
///----------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    std::cout<<"._______________________________________________________________________."<<std::endl;
    std::cout<<"|                                                                       |"<<std::endl;
    std::cout<<"|     #####                #     #                                      |"<<std::endl;
    std::cout<<"|    #     # ######   ##   #  #  #   ##   #      #    # ###### #####    |"<<std::endl;
    std::cout<<"|    #       #       #  #  #  #  #  #  #  #      #   #  #      #    #   |"<<std::endl;
    std::cout<<"|     #####  #####  #    # #  #  # #    # #      ####   #####  #    #   |"<<std::endl;
    std::cout<<"|          # #      ###### #  #  # ###### #      #  #   #      #####    |"<<std::endl;
    std::cout<<"|    #     # #      #    # #  #  # #    # #      #   #  #      #   #    |"<<std::endl;
    std::cout<<"|     #####  ###### #    #  ## ##  #    # ###### #    # ###### #    #   |"<<std::endl;
    std::cout<<"|                                                                       |"<<std::endl;
    std::cout<<"|                            -- V2.0 --                                 |"<<std::endl;
    std::cout<<"|_______________________________________________________________________|"<<std::endl;

    /*
     *  Initialisation of the components
     */
    std::cout<<std::endl<<std::endl<<"...Initialising..."<<std::endl<<std::endl;
    
    // Install custom signal handlers
    install_sig_traps();
    // This is for eclipse development so the output is constantly pumped out.
    setbuf(stdout, NULL);
    // Set exit terminator
    std::atexit(atexit_handler);
    
	/*
     *  Starting main system services
     */
    std::cout<<"1. Setting up main services.........";
    
    //Database Path
	std::string db_path;
	if (argc < 2)
	{
		db_path = "../boat.db";
	}
	else
	{
		db_path = std::string(argv[1]);
	}

	// Declare DBHandler and MessageBus
	DBHandler dbHandler(db_path);
	MessageBus messageBus;

	// Logger start
	Logger::info("Built on %s at %s", __DATE__, __TIME__);
	Logger::info("SeaWalker");
  	#if LOCAL_NAVIGATION_MODULE == 1
		Logger::info( "Using Local Navigation Module" );
  	#else
		Logger::info( "Using Line-follow" );
  	#endif
	Logger::info("Logger init\t\t[OK]");

	// Initialise DBHandler
	if(dbHandler.initialise())
	{
		Logger::info("Database Handler init\t\t[OK]");
	}
	else
	{
		Logger::error("Database Handler init\t\t[FAILED]");
		exit(-1);
	}


	// Declare nodes
	//-------------------------------------------------------------------------------

	int dbLoggerQueueSize = 5; 			// how many messages to log to the databse at a time
	DBLoggerNode dbLoggerNode(messageBus, dbHandler, dbLoggerQueueSize);
	HTTPSyncNode httpsync(messageBus, &dbHandler);
	StateEstimationNode stateEstimationNode(messageBus, dbHandler);
	WindStateNode windStateNode(messageBus);
	WaypointMgrNode waypoint(messageBus, dbHandler);
	CollidableMgr collidableMgr;
	WingSailControlNode wingSailControlNode(messageBus, dbHandler);
	CourseRegulatorNode courseRegulatorNode(messageBus, dbHandler);

  	#if LOCAL_NAVIGATION_MODULE == 1
		LocalNavigationModule lnm	( messageBus, dbHandler );

		const int16_t MAX_VOTES = dbHandler.retrieveCellAsInt("config_voter_system","1","max_vote");
		WaypointVoter waypointVoter( MAX_VOTES, dbHandler.retrieveCellAsDouble("config_voter_system","1","waypoint_voter_weight")); // weight = 1
		WindVoter windVoter( MAX_VOTES, dbHandler.retrieveCellAsDouble("config_voter_system","1","wind_voter_weight")); // weight = 1
		ChannelVoter channelVoter( MAX_VOTES, dbHandler.retrieveCellAsDouble("config_voter_system","1","channel_voter_weight")); // weight = 1
		MidRangeVoter midRangeVoter( MAX_VOTES, dbHandler.retrieveCellAsDouble("config_voter_system","1","midrange_voter_weight"), collidableMgr );
		ProximityVoter proximityVoter( MAX_VOTES, dbHandler.retrieveCellAsDouble("config_voter_system","1","proximity_voter_weight"), collidableMgr);

		lnm.registerVoter( &waypointVoter );
		lnm.registerVoter( &windVoter );
		lnm.registerVoter( &channelVoter );
		lnm.registerVoter( &proximityVoter );
		lnm.registerVoter( &midRangeVoter );
  	#else
		LineFollowNode sailingLogic(messageBus, dbHandler);
  	#endif

	#if SIMULATION == 1
  		SimulationNode simulation(messageBus, 1, &collidableMgr);
  	#else
		CANService canService;

		HMC6343Node compass(messageBus, dbHandler);
	  	GPSDNode gpsd(messageBus, dbHandler);
		CANWindsensorNode windSensor(messageBus, dbHandler, canService);
	  	ActuatorNodeASPire actuators(messageBus, canService);
	  	CANArduinoNode actuatorFeedback(messageBus, dbHandler, canService);

		CANMarineSensorReceiver canMarineSensorReciver(messageBus, canService);

		CANMarineSensorTransmissionNode canMarineSensorTransmissionNode(messageBus, canService);
		CANCurrentSensorNode canCurrentSensorNode(messageBus, dbHandler, canService);

	#endif


	// Initialise nodes
	//-------------------------------------------------------------------------------
    std::cout<<"2. Initialising active nodes........."<<std::endl;

	initialiseNode(httpsync, "Httpsync", NodeImportance::NOT_CRITICAL); // This node is not critical during the developement phase.
	initialiseNode(dbLoggerNode, "DBLogger", NodeImportance::CRITICAL);

	initialiseNode(stateEstimationNode,"StateEstimation",NodeImportance::CRITICAL);
	initialiseNode(windStateNode,"WindState",NodeImportance::CRITICAL);
	initialiseNode(waypoint, "Waypoint", NodeImportance::CRITICAL);

 	initialiseNode(wingSailControlNode, "Wing Sail Controller", NodeImportance::CRITICAL);
 	initialiseNode(courseRegulatorNode, "Course Regulator", NodeImportance::CRITICAL);

	#if LOCAL_NAVIGATION_MODULE == 1
		initialiseNode( lnm, "Local Navigation Module",	NodeImportance::CRITICAL );
	#else
		initialiseNode(sailingLogic, "LineFollow", NodeImportance::CRITICAL);
	#endif

	#if SIMULATION == 1
		initialiseNode(simulation,"Simulation",NodeImportance::CRITICAL);
	#else
		initialiseNode(compass, "Compass", NodeImportance::CRITICAL);
		initialiseNode(gpsd, "GPSD", NodeImportance::CRITICAL);
		initialiseNode(windSensor, "Wind Sensor", NodeImportance::CRITICAL);
		initialiseNode(actuators, "Actuators", NodeImportance::CRITICAL);
		initialiseNode(actuatorFeedback, "Actuator Feedback", NodeImportance::NOT_CRITICAL);
		initialiseNode(canMarineSensorTransmissionNode, "Marine Sensors", NodeImportance::NOT_CRITICAL);
		initialiseNode(canCurrentSensorNode, "Current Sensors", NodeImportance::NOT_CRITICAL);
	#endif

	// Start active nodes
	//-------------------------------------------------------------------------------
    std::cout<<"2. Enabling active nodes........."<<std::endl;

	httpsync.start();
	dbLoggerNode.start();

	stateEstimationNode.start();
	collidableMgr.startGC();

	wingSailControlNode.start();
	courseRegulatorNode.start();

	#if SIMULATION == 1
		simulation.start();
	#else
	  	auto future = canService.start();
		compass.start();
		gpsd.start();
		windSensor.start();
		actuatorFeedback.start();
		canCurrentSensorNode.start();
	#endif

	#if LOCAL_NAVIGATION_MODULE == 1
		lnm.start();
	#else
		sailingLogic.start();
	#endif

	// Begins running the message bus
	//-------------------------------------------------------------------------------
    std::cout<<"3. Starting MessageBus.........";
    try {
        messageBus.run();
        std::cout<<"OK"<<std::endl;
    } catch (std::exception& e)
    {
        std::cout<<"NO"<<std::endl;
        std::cout<<"Detail: "<<e.what()<<std::endl;
    }
    
    // All Systems Online
    //-------------------------------------------------------------------------------
    std::cout<<std::endl<<"...Running..."<<std::endl<<std::endl;

    return 0;
}
