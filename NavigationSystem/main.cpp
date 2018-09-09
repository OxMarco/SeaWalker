/**
 * @file    main.cpp
 *
 * @brief   The entry point for the control system
 *
 */

#ifdef __APPLE__
#define SIMULATION 0
#endif

#include "Database/DBHandler.hpp"
#include "Database/DBLoggerNode.hpp"
#include "HTTPSync/HTTPSyncNode.hpp"
#include "MessageBus/MessageBus.hpp"
#include "SystemServices/Logger.hpp"
#include "WorldState/StateEstimationNode.hpp"
#include "WorldState/WindStateNode.hpp"
#include "WorldState/AISProcessing.hpp"
#include "WorldState/CameraProcessingUtility.hpp"
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
  //#include "Hardwares/HMC6343Node.h"
  #include "Hardwares/GPSDNode.h"
  //#include "Hardwares/CAN_Services/CANService.h"
  #include "Hardwares/CANWindsensorNode.h"
  #include "Hardwares/ActuatorNodeASPire.h"
  #include "Hardwares/CANArduinoNode.h"

    #include "Hardwares/CANMarineSensorReceiver.h"
    #include "Hardwares/CANMarineSensorTransmissionNode.h"
    #include "Hardwares/CANCurrentSensorNode.h"

#endif

#include "Hardwares/AutopilotReadNode.hpp"
#include "Hardwares/AutopilotWriteNode.hpp"

#include "Libs/termcolor/termcolor.hpp"
#include "sigtraps.h"

#include <string>
#include <map>


enum class NodeImportance {
	CRITICAL,
	NOT_CRITICAL
};

MessageBus messageBus;
std::vector<ActiveNode*> nodeList; // store each node and whether was correctly initialised or not

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
		Logger::success("Node: %s - init\t[OK]", nodeName);
        if(node.isActiveNode())
            nodeList.push_back(&dynamic_cast<ActiveNode&>(node));
	}
	else
	{
		Logger::warning("Node: %s - init\t\t[FAILED]", nodeName);

		if(importance == NodeImportance::CRITICAL)
		{
			Logger::error("Critical node failed to initialise, shutting down");
			exit(EXIT_FAILURE);
		}
	}
}

void atexit_handler()
{
    // Shutdown
    //-------------------------------------------------------------------------------
    std::cout<<rang::style::bold<<rang::fg::blue<<std::endl<<std::endl<<"...Exiting..."<<std::endl<<std::endl<<rang::style::reset;

    for (std::vector<ActiveNode*>::iterator it=nodeList.begin(); it!=nodeList.end(); ++it)
    {
        (*it)->stop();
        Logger::success("Node: %s - stopped\t[OK]", (*it)->nodeName().c_str());
    }
    Logger::info("Disabling messageBus");
    messageBus.stop();
}

///----------------------------------------------------------------------------------
/// @brief Entry point, can accept one argument containing a relative path to the database.
///----------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    std::cout<<rang::bg::black<<rang::fg::blue;
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
    std::cout<<rang::style::reset;

    // Initialisation of the components
    //-------------------------------------------------------------------------------
    std::cout<<rang::style::bold<<rang::fg::blue<<std::endl<<std::endl<<"...Initialising..."<<std::endl<<std::endl<<rang::style::reset;
    
    // Install custom signal handlers
    install_sig_traps();
    // Set exit terminator
    std::atexit(atexit_handler);
    
    // Print info
    Logger::info("Run on %s at %s", __DATE__, __TIME__);
    Logger::info("SeaWalker");
#if LOCAL_NAVIGATION_MODULE == 1
    Logger::info( "Using Local Navigation Module" );
#else
    Logger::info( "Using Line-follow" );
#endif
    
    // Starting main system services
    //-------------------------------------------------------------------------------
    std::cout<<rang::style::bold<<rang::fg::blue<<std::endl<<std::endl<<"1. Setting up main services........."<<std::endl<<std::endl<<rang::style::reset;
    
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

	// Declare DBHandler
	DBHandler dbHandler(db_path);

	// Logger start
	Logger::success("Logger init\t\t[OK]");

	// Initialise DBHandler
	if(dbHandler.initialise())
	{
		Logger::success("Database Handler init\t\t[OK]");
	}
	else
	{
		Logger::error("Database Handler init\t\t[FAILED]");
		exit(EXIT_FAILURE);
	}

    // Autopilot interface
    AutopilotInterface autopilot;
    autopilot.open();

    // Collidable Mgr
    CollidableMgr collidableMgr;
    collidableMgr.startGC();
    
    // Initialise active nodes
    //-------------------------------------------------------------------------------
    std::cout<<rang::style::bold<<rang::fg::blue<<std::endl<<std::endl<<"2. Initialising active nodes........."<<std::endl<<std::endl<<rang::style::reset;
   
    // Hardware interfaces
    AutopilotReadNode autopilotRead(messageBus, dbHandler, autopilot);
    initialiseNode(autopilotRead, "AutopilotReadNode", NodeImportance::CRITICAL);
    autopilotRead.start();
    
    AutopilotWriteNode autopilotWrite(messageBus, autopilot);
    initialiseNode(autopilotWrite, "AutopilotWriteNode", NodeImportance::CRITICAL);
    
    // Passive nodes
    WindStateNode windStateNode(messageBus);
    initialiseNode(windStateNode,"WindStateNode",NodeImportance::CRITICAL);
    
    WaypointMgrNode waypointMgr(messageBus, dbHandler);
    initialiseNode(waypointMgr, "WaypointMgrNode", NodeImportance::CRITICAL);

    // Active nodes
    int dbLoggerQueueSize = 5; // how many messages to log to the database at a time
    DBLoggerNode dbLoggerNode(messageBus, dbHandler, dbLoggerQueueSize);
    initialiseNode(dbLoggerNode, "DBLoggerNode", NodeImportance::CRITICAL);

    LineFollowNode sailingLogic(messageBus, dbHandler);
    initialiseNode(sailingLogic, "LineFollowNode", NodeImportance::CRITICAL);
    
    StateEstimationNode stateEstimationNode(messageBus, dbHandler);
    initialiseNode(stateEstimationNode,"StateEstimationNode",NodeImportance::CRITICAL);

    WingSailControlNode wingSailControlNode(messageBus, dbHandler);
    initialiseNode(wingSailControlNode, "WingSailControlNode", NodeImportance::CRITICAL);

    CourseRegulatorNode courseRegulatorNode(messageBus, dbHandler);
    initialiseNode(courseRegulatorNode, "CourseRegulatorNode", NodeImportance::CRITICAL);
    
    HTTPSyncNode httpsync(messageBus, dbHandler);
    initialiseNode(httpsync, "HTTPSyncNode", NodeImportance::NOT_CRITICAL); // This node is not critical during the developement phase.

    AISProcessing aisProcessing(messageBus, dbHandler, &collidableMgr);
    initialiseNode(aisProcessing, "AISProcessing", NodeImportance::CRITICAL);
    
    //CameraProcessingUtility cameraProcessing(messageBus, dbHandler, &collidableMgr);
    //initialiseNode(cameraProcessing, "CameraProcessingUtility", NodeImportance::NOT_CRITICAL);
    
    // Simulator
#if SIMULATION == 1
    SimulationNode simulator(messageBus, 1, &collidableMgr);
    initialiseNode(simulator,"SimulationNode",NodeImportance::CRITICAL);
#endif
    
    // Start active nodes
    //-------------------------------------------------------------------------------
    std::cout<<rang::style::bold<<rang::fg::blue<<std::endl<<std::endl<<"3. Enabling active nodes........."<<std::endl<<std::endl<<rang::style::reset;

    dbLoggerNode.start();
    sailingLogic.start();
    stateEstimationNode.start();
    wingSailControlNode.start();
    courseRegulatorNode.start();
    //httpsync.start();
    aisProcessing.start();
    //cameraProcessing.start();
    
#if SIMULATION == 1
    simulator.start();
#endif
    
    /*

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
     
     */

    std::this_thread::sleep_for(std::chrono::seconds(1));

	// Begins running the message bus
	//-------------------------------------------------------------------------------
    std::cout<<rang::style::bold<<rang::fg::blue<<std::endl<<std::endl<<"4. Starting MessageBus........."<<std::endl<<std::endl<<rang::style::reset;

    try {
        messageBus.run();
        std::cout<<"OK"<<std::endl;
    } catch (std::exception& e)
    {
        std::cout<<"NO"<<std::endl;
        std::cout<<"Detail: "<<e.what()<<std::endl;
    }

    return 0;
}
