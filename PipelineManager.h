#ifndef PIPELINE_MANAGER_HPP
#define PIPELINE_MANAGER_HPP

#include "depthai/depthai.hpp"

class PipelineManager {
private:
    dai::Pipeline pipeline;
    std::shared_ptr<dai::node::MonoCamera> monoLeft;
    std::shared_ptr<dai::node::MonoCamera> monoRight;
    std::shared_ptr<dai::node::StereoDepth> stereo;
    std::shared_ptr<dai::node::SpatialLocationCalculator> spatialLocationCalculator;
    std::shared_ptr<dai::node::XLinkOut> xoutDepth;
    std::shared_ptr<dai::node::XLinkOut> xoutSpatialData;
    std::shared_ptr<dai::node::XLinkIn> xinSpatialCalcConfig;

    void configureNodes();

public:
    PipelineManager();
    dai::Pipeline getPipeline() const;
};

#endif
