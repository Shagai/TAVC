#pragma once

#include <iostream>
#include <stdio.h>

#include "opencv2\flann\flann.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2\features2d\features2d.hpp"
#include "opencv2\xfeatures2d.hpp"

using namespace cv;

class Classifier
{
public:

    CascadeClassifier phone1_cascade;

    // Methods
    Classifier(String fname);
    ~Classifier();

    std::vector<Rect> Classifier::ImageDetection(Mat frame);
    void Classifier::DrawMarks(std::vector<Rect> mark, Mat frame);
	void Classifier::CheckDetection(std::vector<Rect> &mark, Mat frame);
	void Classifier::FeatureDetection(std::vector<Rect> marks, Mat frame);

private:

	// Detect Features
	int _prevNumMarks = 0;
	Mat _object;
	Ptr<xfeatures2d::SURF> _detector;
	std::vector<KeyPoint> _keypoints_object;
	Mat _descriptors_object;
};

