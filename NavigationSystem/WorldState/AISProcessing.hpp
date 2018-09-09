/**
 * @file    AISProcessing.hpp
 *
 * @brief   Receives the AIS data from the CANAISNode, processes it and adds the vessels
 *          that are in a certain radius to the collidableMgr
 *
 */

#ifndef AISPROCESSING_HPP
#define AISPROCESSING_HPP

#include "../Database/DBHandler.hpp"
#include "../Math/CourseMath.hpp"
#include "../MessageBus/ActiveNode.hpp"
#include "../MessageBus/Message.hpp"
#include "../Messages/AISDataMsg.hpp"
#include "../Messages/StateMessage.h"
#include "../SystemServices/Logger.hpp"
#include "../SystemServices/Timer.hpp"
#include "../WorldState/CollidableMgr/CollidableMgr.h"

#include <chrono>
#include <mutex>
#include <thread>
#include <vector>

class AISProcessing : public ActiveNode {
   public:
    /**
     * @brief Constructor, pointer to messagebus and canservice
     * int radius, the distance [meter] from us in which a vessel is interesting
     * uint32_t mmsi, the ID number of our shown vessel, makes sure it gets ignored and not added to
     * the collidable manager double loopTime, how often we send messages
     */
    AISProcessing(MessageBus& msgBus, DBHandler& dbhandler, CollidableMgr* collidableMgr);
    ~AISProcessing();

    bool init();

    /**
     * @brief Processes the message received
     */
    void processMessage(const Message* msg);

    /**
     * @brief Starts the worker thread
     */
    void start();

    /**
     * @brief Stops the worker thread
     */
    void stop();

   private:
    /**
     * @brief Update values from the database as the loop time of the thread and others parameters
     */
    void updateConfigsFromDB();

    /**
     * @brief Sends the data to the collidable manager
     */
    void addAISDataToCollidableMgr();

    /**
     * @brief Gets to process the message if the message received is an AISDataMsg
     */
    void processAISMessage(AISDataMsg* msg);

    /**
     * @brief The function that thread works on
     */
    static void AISProcessingThreadFunc(ActiveNode* nodePtr);

    /**
     * @brief Private variables
     */
    std::vector<AISVessel> m_Vessels;
    std::vector<AISVesselInfo> m_InfoList;
    double m_latitude;
    double m_longitude;
    double m_LoopTime;
    int m_Radius;
    uint32_t m_MMSI;
    std::mutex m_lock;
    CollidableMgr* collidableMgr;
    DBHandler& m_db;
    std::atomic<bool> m_running;
};

#endif /* AISPROCESSING_HPP */
