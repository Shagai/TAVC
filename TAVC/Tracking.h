#pragma once
#include "opencv2\tracking.hpp"
#include "opencv2/core.hpp"

using namespace cv;

class Tracking
{

public:

    // Attributes
	MultiTracker _trackers;
    Rect2d roi;

    // Methods
	Tracking();
    ~Tracking();

    void InitializeTracking(Mat frame, Rect roi);
    int UpdateTracking(Mat frame);
};

