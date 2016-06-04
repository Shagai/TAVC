#pragma once

#include <vector>

#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/tracking.hpp"
#include "opencv2\video\video.hpp"


#include "opencv2/xfeatures2d.hpp"

using namespace cv;
using namespace std;

class Odometry
{
private:
    Mat _prevFrame;
    vector<Point2f> _prevFeatures;
    uint _state = 0;
    Mat _t_f, _R_f;
    Mat _traj = Mat::zeros(600, 600, CV_8UC3);

    void featureTracking(Mat img_1, Mat img_2, vector<Point2f>& points1,
                         vector<Point2f>& points2);
    void featureDetection(Mat img_1, vector<Point2f>& points1);

	void DrawPath();

public:
    Odometry(Mat frame);
    ~Odometry();

    void Update(Mat frame);

    void Odometry::SetFrame(Mat frame);
};
