#include "Tracking.h"


Tracking::Tracking(String trackerType){
    this->tracker = Tracker::create(trackerType);
}


Tracking::~Tracking(){

}

// initialize the tracker
void Tracking::InitializeTracking(Mat frame, Rect2d roi){

    this->roi = roi;
    this->tracker->init(frame, roi);
}

void Tracking::UpdateTracking(Mat frame){

    // update the tracking result
    this->tracker->update(frame, roi);
    // draw the tracked object
    rectangle(frame, roi, Scalar(255, 0, 0), 2, 1);
}
