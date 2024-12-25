#ifndef DISPLAY_MANAGER_HPP
#define DISPLAY_MANAGER_HPP

#include "PipelineManager.h"
#include "LogicManager.h"
#include "WebSocketClient.h"
#include <opencv2/opencv.hpp>

class DisplayManager {
private:
    PipelineManager pipelineManager;
    LogicManager logicManager;
    WebSocketClient webSocketClient;

    dai::Device device;
    std::shared_ptr<dai::DataOutputQueue> depthQueue;
    std::shared_ptr<dai::DataOutputQueue> spatialCalcQueue;

    void processFrame();
    void drawROIs(cv::Mat& frame, const std::vector<dai::SpatialLocations>& spatialData);
    void logDistanceGrid();  // Thêm phương thức để log ma trận distanceGrid

public:
    DisplayManager();
    void run();
};

#endif
