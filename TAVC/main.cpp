#include <iostream>
#include <stdio.h>

#include "opencv2\core.hpp"
#include "opencv2/tracking.hpp"
#include "opencv2\videoio.hpp"
#include "opencv2\highgui.hpp"

#include "Detection.h"
#include "Odometry.h"

using namespace std;
using namespace cv;

/* @function main */
int main(void) {

    // Set Device
    cv::VideoCapture capture("desarrollo.mp4");

    // Create main Mats
    cv::Mat frame, frame_dst;
    Rect2d roi;

    // First frame is black
	for (int i = 0; i < 2; i++){ 
		capture >> frame;
	}

	Detection detector("cascade_logo.xml");
    Odometry odom(frame);

    for (;;) {

        // Get current frame from VideoCapture
        capture >> frame;
        if (frame.empty())
            break;

        // Update Odom
        //odom.Update(frame);

		// Update Detection and Tracking
		detector.Update(frame, Scalar(95, 125, 85), Scalar(115, 255, 255));

        // Show Image
        cv::imshow("tracker", frame);

        // Avoid Gray Screen
        int c = cv::waitKey(10);
        if ((char)c == 27) { break; } // escape
    }

    return 0;
}