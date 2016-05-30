#pragma once

#include <iostream>
#include <stdio.h>

#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/objdetect.hpp"

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
};

