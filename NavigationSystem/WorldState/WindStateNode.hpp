/**
 * @file    WindStateNode.hpp
 *
 * @brief   Handles wind messages to calculate true and apparent wind speed.
 
 * @details Each time a vessel state message is received:
 *          - Calculates the instantaneous true wind (speed and direction) from wind sensor and Vessel
 *              State datas.
 *          - Returns a WindStateMsg corresponding to the true and apparent wind state (speed and
 *              direction). The wind direction corresponds to the direction where the wind comes from.
 *
 */

#ifndef WINDSTATENODE_HPP
#define WINDSTATENODE_HPP

#include "../Database/DBHandler.hpp"
#include "../Math/Utility.hpp"
#include "../MessageBus/MessageBus.hpp"
#include "../MessageBus/MessageTypes.hpp"
#include "../Messages/StateMessage.h"
#include "../Messages/WindDataMsg.h"
#include "../Messages/WindStateMsg.h"
#include <mutex>
#include <vector>

class WindStateNode : public Node {
   public:
    WindStateNode(MessageBus& msgBus);
    ~WindStateNode();

    bool init();
    void processMessage(const Message* message);

   private:
    ///----------------------------------------------------------------------------------
    /// @brief Stores vessel state datas from a StateMessage.
    ///----------------------------------------------------------------------------------
    void processVesselStateMessage(const StateMessage* msg);

    ///----------------------------------------------------------------------------------
    /// @brief Stores apparent wind datas from a WindDataMsg.
    ///----------------------------------------------------------------------------------
    void processWindMessage(const WindDataMsg* msg);

    ///----------------------------------------------------------------------------------
    /// @brief Sends windStateMsg.
    ///----------------------------------------------------------------------------------
    void sendMessage();

    ///----------------------------------------------------------------------------------
    /// @brief Calculates the instantaneous true wind (speed and direction).
    ///----------------------------------------------------------------------------------
    void calculateTrueWind();

    float m_vesselHeading;          ///< heading in degrees [0, 360) in North-East reference frame (clockwise)
    float m_vesselSpeed;            ///< speed in m/s
    float m_vesselCourse;           ///< course in degrees [0, 360) in  North-East reference frame (clockwise)
    float m_apparentWindSpeed;      ///< apparent wind speed in m/s
    float m_apparentWindDirection;  ///< apparent wind direction in degrees [0, 360) in vessel reference frame (clockwise)
    float m_trueWindSpeed;      ///< true wind speed in m/s
    float m_trueWindDirection;  ///< true wind direction in degrees [0, 360) in North-East reference frame (clockwise)
};

#endif /* WINDSTATENODE_HPP */
