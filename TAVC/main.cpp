#include <iostream>
#include <stdio.h>

#include "opencv2\core.hpp"
#include "opencv2/tracking.hpp"
#include "opencv2\videoio.hpp"
#include "opencv2\highgui.hpp"

#include "Classifier.h"
#include "Tracking.h"
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
    //roi = selectROI("tracker", frame);

    Classifier classif("cascade.xml");

	// set the default tracking algorithm
	Tracking trackers;

    Odometry odom(frame);
    uint h = 0;

	int prevNumMarks = 0;

    for (;;) {

        // Get current frame from VideoCapture
        capture >> frame;
        if (frame.empty())
            break;

        // Update Odom
        /*h++;
        if (h == 1){
            odom.Update(frame);
            h = 0;
        }*/

        // Show Image
        //cv::imshow("Input", frame);

        // Main program
        std::vector<Rect> marks = classif.ImageDetection(frame);
		classif.CheckDetection(marks, frame);

		for (std::vector<Rect>::iterator it = marks.begin(); 
			it != marks.end() && (marks.size() > prevNumMarks); ++it){

				trackers.InitializeTracking(frame, *it);
		}

		prevNumMarks = trackers.UpdateTracking(frame);

        // Draw Marks
		//classif.DrawMarks(marks, frame);

        // Show Image
        cv::imshow("tracker", frame);

        // Avoid Gray Screen
        int c = cv::waitKey(10);
        if ((char)c == 27) { break; } // escape
    }

    return 0;
}