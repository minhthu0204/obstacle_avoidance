#include "PipelineManager.h"

PipelineManager::PipelineManager()
{
    monoLeft = pipeline.create<dai::node::MonoCamera>();
    monoRight = pipeline.create<dai::node::MonoCamera>();
    stereo = pipeline.create<dai::node::StereoDepth>();
    spatialLocationCalculator = pipeline.create<dai::node::SpatialLocationCalculator>();
    xoutDepth = pipeline.create<dai::node::XLinkOut>();
    xoutSpatialData = pipeline.create<dai::node::XLinkOut>();
    xinSpatialCalcConfig = pipeline.create<dai::node::XLinkIn>();

    configureNodes();
}

void PipelineManager::configureNodes(){
    // Configure Mono Cameras
    monoLeft->setResolution(dai::MonoCameraProperties::SensorResolution::THE_400_P);
    monoLeft->setCamera("left");
    monoRight->setResolution(dai::MonoCameraProperties::SensorResolution::THE_400_P);
    monoRight->setCamera("right");

    // Configure StereoDepth
    stereo->setDefaultProfilePreset(dai::node::StereoDepth::PresetMode::HIGH_DENSITY);
    stereo->setLeftRightCheck(true);
    stereo->setSubpixel(true);

    // Configure SpatialLocationCalculator
    spatialLocationCalculator->inputConfig.setWaitForMessage(false);

    // Configure XLinkOut
    xoutDepth->setStreamName("depth");
    xoutSpatialData->setStreamName("spatialData");
    xinSpatialCalcConfig->setStreamName("spatialCalcConfig");

    // Link nodes
    monoLeft->out.link(stereo->left);
    monoRight->out.link(stereo->right);
    stereo->depth.link(spatialLocationCalculator->inputDepth);
    spatialLocationCalculator->passthroughDepth.link(xoutDepth->input);
    spatialLocationCalculator->out.link(xoutSpatialData->input);
    xinSpatialCalcConfig->out.link(spatialLocationCalculator->inputConfig);

    // Add 25 ROIs (5x5 grid)
    static constexpr int GRID_SIZE = 5;
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            dai::SpatialLocationCalculatorConfigData config;
            config.depthThresholds.lowerThreshold = 200;
            config.depthThresholds.upperThreshold = 30000;
            config.calculationAlgorithm = dai::SpatialLocationCalculatorAlgorithm::MEDIAN;
            config.roi = dai::Rect(
                dai::Point2f(i * 0.2f, j * 0.2f),
                dai::Point2f((i + 1) * 0.2f, (j + 1) * 0.2f)
                );
            spatialLocationCalculator->initialConfig.addROI(config);
        }
    }
}

dai::Pipeline PipelineManager::getPipeline() const {
    return pipeline;
}
