#pragma once

#include <iostream>
#include <stdio.h>

#include "opencv2/calib3d.hpp"
#include "opencv2\flann\flann.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2\features2d\features2d.hpp"
#include "opencv2\xfeatures2d.hpp"

using namespace cv;
using namespace cv::xfeatures2d;

class Classifier
{
public:

    CascadeClassifier _cascade;

    // Methods
	Classifier(String fname);
	Classifier(String fileName, int minHessian);
    ~Classifier();

	void Classifier::Init(String fname);

    std::vector<Rect> Classifier::ImageDetection(Mat frame);
    void Classifier::DrawMarks(std::vector<Rect> mark, Mat frame);
	void Classifier::CheckDetection(std::vector<Rect> &mark, Mat frame, 
									Scalar minRange, Scalar maxRange);
	Rect Classifier::FeatureDetection(Mat frame);
	bool Classifier::CheckDetection(Rect mark, Mat frame, Scalar minRange, Scalar maxRange);

private:

	// Detect Features
	int _prevNumMarks = 0;
	Ptr<SURF> _detector;
	Mat _img_object;
	std::vector<KeyPoint> _keypoints_object;
	Mat _descriptors_object;
};

