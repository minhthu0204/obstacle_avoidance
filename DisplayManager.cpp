#include "DisplayManager.h"

DisplayManager::DisplayManager()
    : device(pipelineManager.getPipeline()),
    webSocketClient(QUrl("ws://192.168.1.48:3335")){
    device.setIrLaserDotProjectorBrightness(1000);

    depthQueue = device.getOutputQueue("depth", 4, false);
    spatialCalcQueue = device.getOutputQueue("spatialData", 4, false);
}

void DisplayManager::processFrame() {
    auto inDepth = depthQueue->get<dai::ImgFrame>();
    cv::Mat depthFrame = inDepth->getFrame();

    // Normalize and apply color map
    cv::Mat depthFrameColor;
    cv::normalize(depthFrame, depthFrameColor, 0, 255, cv::NORM_MINMAX, CV_8UC1);
    cv::applyColorMap(depthFrameColor, depthFrameColor, cv::COLORMAP_HOT);

    auto spatialData = spatialCalcQueue->get<dai::SpatialLocationCalculatorData>()->getSpatialLocations();
    logicManager.processSpatialData(spatialData, depthFrameColor.cols, depthFrameColor.rows);

    drawROIs(depthFrameColor, spatialData);

    // Display action decision
    auto action = logicManager.decideAction();
    //std::cout << "Action: " << action << std::endl;
    QByteArray dataBuffer = QString::fromStdString(action).toUtf8();
    webSocketClient.sendMessage(dataBuffer);

    // Log the distanceGrid
    logDistanceGrid();
    // Show the frame
    cv::imshow("depth", depthFrameColor);
}

void DisplayManager::drawROIs(cv::Mat& frame, const std::vector<dai::SpatialLocations>& spatialData) {
    for (const auto& data : spatialData) {
        auto roi = data.config.roi.denormalize(frame.cols, frame.rows);
        auto coords = data.spatialCoordinates;
        float distance = coords.z;

        int xmin = static_cast<int>(roi.topLeft().x);
        int ymin = static_cast<int>(roi.topLeft().y);
        int xmax = static_cast<int>(roi.bottomRight().x);
        int ymax = static_cast<int>(roi.bottomRight().y);

        cv::Scalar color = (distance / 2000.0f < 1.0f) ? cv::Scalar(0, 0, 255) : cv::Scalar(0, 255, 0);
        cv::rectangle(frame, cv::Rect(cv::Point(xmin, ymin), cv::Point(xmax, ymax)), color, 2);
    }
}


void DisplayManager::logDistanceGrid() {
    // Log the distanceGrid (from LogicManager)
    const auto& distanceGrid = logicManager.getDistanceGrid(); // Lấy distanceGrid từ LogicManager
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            std::cout << distanceGrid[i][j] << " ";  // In giá trị của mỗi ô trong ma trận
        }
        std::cout << std::endl;  // In dòng mới sau khi hoàn thành một hàng
    }
}

void DisplayManager::run() {
    webSocketClient.connectToServer();
    while (true) {
        processFrame();
        if (cv::waitKey(1) == 'q') break;
    }
}
