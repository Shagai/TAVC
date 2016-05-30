#pragma once
#include "opencv2\tracking.hpp"
#include "opencv2/core.hpp"

using namespace cv;

class Tracking
{
public:

    // Attributes
    Ptr<Tracker> tracker;
    Rect2d roi;

    // Methods
    Tracking(String trackerType);
    ~Tracking();

    void InitializeTracking(Mat frame, Rect2d roi);
    void UpdateTracking(Mat frame);
};

