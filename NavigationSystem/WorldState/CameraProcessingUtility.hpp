/**
 * @file    CameraProcessingUtility.cpp
 *
 * @brief   Utility functions for image acquisition and processing.
 *
 * @details While running, this thread can be restarted with 'ESC', or
 *           shut down with 'q'.
 *          TO DO: Add compass correction. Decrease computation time for the Raspberry
 *          (deleting the "imshow" functions can decrease by 0,15s, on the desk computer)
 *          Delete the includes and put it all in the corresponding .h file.
 *
 */

#ifndef CAMERAPROCESSINGUTILITY_HPP
#define CAMERAPROCESSINGUTILITY_HPP

#include "../SystemServices/SysClock.hpp"
#include "../SystemServices/Timer.hpp"
#include "../Database/DBHandler.hpp"
#include "../Messages/CompassDataMsg.h"
#include "../MessageBus/MessageTypes.hpp"
#include "../MessageBus/MessageBus.hpp"
#include "../MessageBus/ActiveNode.hpp"
#include "../SystemServices/Logger.hpp"
#include "CollidableMgr/CollidableMgr.h"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/photo.hpp>
#include <opencv2/videostab.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/video.hpp>
#include <vector>
#include <thread>
#include <mutex>
#include <map>

class CameraProcessingUtility : public ActiveNode {
public:
    CameraProcessingUtility(MessageBus& msgBus, DBHandler& dbhandler, CollidableMgr* collidableMgr);
    ~CameraProcessingUtility();
    
    bool init();
    void start();
    void stop();
    
    cv::Mat getRoi();
    
    std::map<int16_t, uint16_t> getRelDistances();
    
    void processMessage(const Message* msg);
    
private:
    void freeSpaceProcessing();
    int computeRelDistances();
    void addCameraDataToCollidableMgr();
    static void CameraProcessingUtilityThreadFunc(ActiveNode* nodePtr);
    void processCompassMessage(const CompassDataMsg* msg);
    
    cv::VideoCapture m_capture;
    cv::Mat m_imgFullSize;
    cv::Mat m_freeSpaceFrame;
    std::map<int16_t, uint16_t> m_relBearingToRelObstacleDistance;
    
    int m_cameraDeviceID;
    const int m_detectorLoopTime;
    const int m_maxCompassFrameTimeframe;
    CollidableMgr* collidableMgr;
    DBHandler& m_db;
    std::atomic<bool> m_Running;
    std::mutex m_lock;
    
};

#endif /* CAMERAPROCESSINGUTILITY_HPP */
