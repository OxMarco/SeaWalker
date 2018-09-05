/**
 * @file    SailSpeedRegulatorNode.hpp
 * 
 * @brief   Calculates the desired sail angle.
 *          It sends a SailComandMsg corresponding to the command angle of the sail.
 *
 * @details This node has not been tested and will probably not work in reality because the speed
 *          regulator is base on the simulator behaviour which is a rough representation of the reality.
 *
 */

#ifndef SAILSPEEDREGULATORNODE_HPP
#define SAILSPEEDREGULATORNODE_HPP

#include "../Database/DBHandler.hpp"
#include "../Math/Utility.hpp"
#include "../SystemServices/Timer.hpp"
#include "../MessageBus/ActiveNode.hpp"
#include "../MessageBus/MessageBus.hpp"
#include "../Messages/SailCommandMsg.h"
#include "../Messages/StateMessage.h"
#include "../Messages/WindDataMsg.h"
#include <math.h>
#include <stdint.h>
#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>
#include <vector>

class SailSpeedRegulatorNode : public ActiveNode {
   public:
    SailSpeedRegulatorNode(MessageBus& msgBus, DBHandler& dbhandler);
    ~SailSpeedRegulatorNode();

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
    /// @brief Stores vessel speed and course datas from a StateMessage.
    ///----------------------------------------------------------------------------------
    void processStateMessage(const StateMessage* msg);

    ///----------------------------------------------------------------------------------
    /// @brief Limits the command sail angle between m_MaxSailAngle and m_MinSailAngle.
    ///----------------------------------------------------------------------------------
    float restrictSailAngle(float val);

    float speedRegulator(float val);

    ///----------------------------------------------------------------------------------
    /// @brief Calculate the sail angle according to a linear relation to the apparent wind direction.
    ///----------------------------------------------------------------------------------
    float calculateSailAngleLinear();

    ///----------------------------------------------------------------------------------
    /// @brief Calculate the sail angle according to a cardioid relation to the apparent wind direction.
    ///----------------------------------------------------------------------------------
    float calculateSailAngleCardioid();

    ///----------------------------------------------------------------------------------
    /// @brief Starts the SailSpeedRegulatorNode's thread that pumps out SailCommandMsg.
    ///----------------------------------------------------------------------------------
    static void SailSpeedRegulatorNodeThreadFunc(ActiveNode* nodePtr);

    DBHandler& m_db;
    std::mutex m_lock;
    std::atomic<bool> m_Running;

    double m_LoopTime;      // seconds
    double m_MaxSailAngle;  // degrees
    double m_MinSailAngle;  // degrees

    double m_ApparentWindDir;  // degrees [0, 360[ in North-East reference frame (clockwise)
    float m_VesselSpeed;       // m/s

    float m_old_diff_v;  // m/s
    float m_int_diff_v;
};

#endif /* SAILSPEEDREGULATORNODE_HPP */
