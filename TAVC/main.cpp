#include <iostream>
#include <stdio.h>

#include "opencv2\core.hpp"
#include "opencv2/tracking.hpp"
#include "opencv2\videoio.hpp"
#include "opencv2\highgui.hpp"

#include "Classifier.h"
#include "Odometry.h"
#include "Tracking.h"

#include <time.h>
#include <chrono>

using namespace std;
using namespace cv;
using namespace std::chrono;

/* @function main */
int main(void) {

    // Set Device
    cv::VideoCapture capture("FINAL.mp4");
	//namedWindow("tracker", WINDOW_AUTOSIZE);
	double fps = capture.get(CV_CAP_PROP_FPS);

    // Create main Mats
    cv::Mat frame, frame_dst;
    Rect2d roi;

    // First frame is black
	for (int i = 0; i < 2; i++){ 
		capture >> frame;
	}

	//Detection detector1("cascade.xml");
    Odometry odom(frame);
	//FindObject find("tlf1.jpg", 1000);

	Classifier logo("cascade_logo.xml");
	Classifier tlf("tlf1.jpg", 1000);

	Tracking trackers;

	int prevNumTracks = 0;
	//int countFrame = 0;

    for (;;) {

		// Update Odom
		odom.Update(frame);

		//countFrame++;
		//if (countFrame % 2 == 0) continue;
		// Start time
		high_resolution_clock::time_point t1 = high_resolution_clock::now();
		
        // Get current frame from VideoCapture
        capture >> frame;
		resize(frame, frame, Size(1280, 720));
        if (frame.empty())
            break;
		
		high_resolution_clock::time_point t4 = high_resolution_clock::now();
		int prevNumTracks = trackers.GetNumberOfTrackers();
		int currNumTracks = trackers.UpdateTracking(frame);
		if (prevNumTracks > currNumTracks /*&& currNumTracks == 0*/) trackers.~Tracking();
		
		high_resolution_clock::time_point t5 = high_resolution_clock::now();
		std::cout << "Track Update: " << duration_cast<milliseconds>(t5 - t4).count() << endl;
		
		std::vector<Rect> marks;
		if (countFrame == 1){
			high_resolution_clock::time_point t0 = high_resolution_clock::now();
			marks = logo.ImageDetection(frame);
			logo.CheckDetection(marks, frame, Scalar(95, 125, 125), Scalar(115, 160, 255));

			high_resolution_clock::time_point t1 = high_resolution_clock::now();
			std::cout << "Cascade: " << duration_cast<milliseconds>(t1 - t0).count() << endl;
			//countFrame = -5;
		}

		

		if (countFrame == 5){
			high_resolution_clock::time_point t2 = high_resolution_clock::now();
			

			Rect mark = tlf.FeatureDetection(frame);
			if (mark.height > 0 && mark.width > 0){
				//std::cout << "width: " << mark.width << " height: " << mark.height << std::endl;
				//std::cout << "x: " << mark.x << " y: " << mark.y << std::endl;
				if (tlf.CheckDetection(mark, frame, Scalar(15, 30, 60), Scalar(130, 130, 190))){
					marks.push_back(mark);
				}
			}
			countFrame = -1;

			high_resolution_clock::time_point t3 = high_resolution_clock::now();
			std::cout << "SURF: " << duration_cast<milliseconds>(t3 - t2).count() << endl;
		}
		
		//tlf.DrawMarks(marks, frame);
		for (std::vector<Rect>::iterator it = marks.begin();
			it != marks.end(); ++it){
			//std::cout << "x init: " << it->x << " y init: " << it->y << std::endl;
			trackers.InitializeTracking(frame, *it);
		}
	
		//std::cout << "Num trackers: " << prevNumMarks << endl;
		//std::cout << "FPS: " << 1.0 / duration_cast<milliseconds>(t2 - t1).count() * 1000 << endl;

        // Show Image
        cv::imshow("tracker", frame);

		
        // Avoid Gray Screen
        int c = cv::waitKey(10);
        if ((char)c == 27) { break; } // escape
    }

    return 0;
}