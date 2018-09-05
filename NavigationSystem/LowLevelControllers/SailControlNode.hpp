/**
 * @file   SailControlNode.hpp
 * @version 1.0.0
 * @author SailingRobots team
 * @date   2017
 * @brief  Calculates the desired sail angle.
 *         It sends a SailComandMsg corresponding to the command angle of the sail.
 * @details Two functions have been developed to calculate the desired sail angle :
 *          - calculateSailAngleLinear(),
 *          - calculateSailAngleCardioid().
 *          The developer can choose the one you want to use by commenting/uncommenting lines
 *          in SailControlNodeThreadFunc().
 *
 */

#ifndef SAILCONTROLNODE_HPP
#define SAILCONTROLNODE_HPP

#include "../Database/DBHandler.hpp"
#include "../Math/Utility.hpp"
#include "../SystemServices/Timer.hpp"
#include "../MessageBus/ActiveNode.hpp"
#include "../MessageBus/MessageBus.hpp"
#include "../Messages/SailCommandMsg.h"
#include "../Messages/WindDataMsg.h"
#include <math.h>
#include <stdint.h>
#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>
#include <vector>

class SailControlNode : public ActiveNode {
   public:
    SailControlNode(MessageBus& msgBus, DBHandler& dbhandler);
    ~SailControlNode();

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
    /// @brief Limits the command sail angle between m_MaxSailAngle and m_MinSailAngle.
    ///----------------------------------------------------------------------------------
    float restrictSailAngle(float val);

    ///----------------------------------------------------------------------------------
    /// @brief Calculates the sail angle according to a linear relation to the apparent wind direction.
    ///----------------------------------------------------------------------------------
    float calculateSailAngleLinear();

    ///----------------------------------------------------------------------------------
    /// @brief Calculates the sail angle according to a cardioid relation to the apparent wind direction.
    ///----------------------------------------------------------------------------------
    float calculateSailAngleCardioid();

    ///----------------------------------------------------------------------------------
    /// @brief Starts the SailControlNode's thread that pumps out SailCommandMsg.
    ///----------------------------------------------------------------------------------
    static void SailControlNodeThreadFunc(ActiveNode* nodePtr);

    DBHandler& m_db; ///< a handle to the db
    std::mutex m_lock; ///< avoid racing conditions
    std::atomic<bool> m_Running; ///< stores the status of the node

    double m_LoopTime;      ///< loop time in seconds
    double m_MaxSailAngle;  ///< max sail angle in degrees
    double m_MinSailAngle;  ///< min sail angle in degrees

    double m_ApparentWindDir;  ///< apparent wind direction in degrees [0, 360) in North-East reference frame (clockwise)
};

#endif /* SAILCONTROLNODE_HPP */
