#include <iostream>
#include <stdio.h>

#include "opencv2\core.hpp"
#include "opencv2\core\cuda.hpp"
#include "opencv2\videoio.hpp"
#include "opencv2\highgui.hpp"

using namespace std;
using namespace cv;

/* @function main */
int main(void) {

	// Set Device
	cv::cuda::setDevice(0);
	cv::VideoCapture capture("desarrollo.mp4");

	// Create main Mats
	cv::cuda::GpuMat frame_d, result;
	cv::Mat frame_h;
	cv::Mat frame_dst;

	for (;;) {

		// Get current frame from VideoCapture
		capture >> frame_h;
		if (frame_h.empty())
			break;

		// From host to device
		frame_d.upload(frame_h);

		// From device to host
		frame_d.download(frame_dst);

		// Show Image
		cv::imshow("Output", frame_dst);

		int c = cv::waitKey(10);
		if ((char)c == 27) { break; } // escape
	}

	return 0;
}