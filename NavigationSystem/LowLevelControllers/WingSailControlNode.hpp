/**
 * @file    WingSailControlNode.hpp
 * 
 * @brief   Calculates the desired tail wing angle of the wingsail.
 *          It sends a WingSailComandMsg corresponding to the command angle of the tail wing.
 *
 * @details Two functions have been developed to calculate the desired tail angle :
 *          - calculateTailAngle(),
 *          - simpleCalculateTailAngle().
 *          You can choose the one you want to use by commenting/uncommenting lines
 *          in WingSailControlNodeThreadFunc().
 */

#ifndef WINGSAILCONTROLNODE_HPP
#define WINGSAILCONTROLNODE_HPP

#include "../Database/DBHandler.hpp"
#include "../Math/Utility.hpp"
#include "../SystemServices/Timer.hpp"
#include "../MessageBus/ActiveNode.hpp"
#include "../MessageBus/MessageBus.hpp"
#include "../Messages/LocalNavigationMsg.h"
#include "../Messages/StateMessage.h"
#include "../Messages/WindStateMsg.hpp"
#include "../Messages/WingSailCommandMsg.hpp"
#include <math.h>
#include <stdint.h>
#include <chrono>
#include <mutex>
#include <atomic>
#include <thread>
#include <vector>

class WingSailControlNode : public ActiveNode {
   public:
    WingSailControlNode(MessageBus& msgBus, DBHandler& dbhandler);
    ~WingSailControlNode();

    bool init();
    void start();
    void stop();
    void processMessage(const Message* message);

   private:
    ///----------------------------------------------------------------------------------
    /// @brief Updates the values of the parameters from the database.
    ///----------------------------------------------------------------------------------
    void updateConfigsFromDB();

    ///----------------------------------------------------------------------------------
    /// @brief Stores apparent wind direction from a WindStateMsg.
    ///----------------------------------------------------------------------------------
    void processWindStateMessage(const WindStateMsg* msg);

    ///----------------------------------------------------------------------------------
    /// @brief Stores target course and tack data from a LocalNavigationMsg.
    ///----------------------------------------------------------------------------------
    void processLocalNavigationMessage(const LocalNavigationMsg* msg);

    ///----------------------------------------------------------------------------------
    /// @brief Limits the command tail angle to m_MaxCommandAngle.
    ///----------------------------------------------------------------------------------
    double restrictWingsail(double val);

    ///----------------------------------------------------------------------------------
    /// @brief Calculates the angle to give to the tail to have maximum force toward boat heading.
    ///        The parameters used by this function have been calculated by CFD simulation. It is
    ///        possible that the values of these parameters do not describe the real wing sail behaviour.
    ///----------------------------------------------------------------------------------
    float calculateTailAngle();

    ///----------------------------------------------------------------------------------
    /// @brief Sets the tail command angle to +/- m_MaxCommandAngle in function of the desired tack of the
    ///        vessel.
    ///----------------------------------------------------------------------------------
    float simpleCalculateTailAngle();

    ///----------------------------------------------------------------------------------
    /// @brief Starts the WingSailControlNode's thread that pumps out WingSailCommandMsg.
    ///----------------------------------------------------------------------------------
    static void WingSailControlNodeThreadFunc(ActiveNode* nodePtr);

    DBHandler& m_db;
    std::mutex m_lock;
    std::atomic<bool> m_Running;

    double m_LoopTime;         // seconds
    double m_MaxCommandAngle;  // degrees

    double m_ApparentWindDir;    // degrees [0, 360[ in North-East reference frame (clockwise)
    float m_TargetCourse;        // degree [0, 360[ in North-East reference frame (clockwise)
    bool m_TargetTackStarboard;  // True if the desired tack of the vessel is starboard.
};

#endif /* WINGSAILCONTROLNODE_HPP */

