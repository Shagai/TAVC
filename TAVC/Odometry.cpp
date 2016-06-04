#include "Odometry.h"

Odometry::Odometry(Mat frame){
	_prevFrame = frame.clone();
}


Odometry::~Odometry(){
}

void Odometry::Update(Mat currentframe){

    // we work with grayscale images
    Mat lframe_gray, cframe_gray;
    cvtColor(_prevFrame, lframe_gray, COLOR_BGR2GRAY);
    cvtColor(currentframe, cframe_gray, COLOR_BGR2GRAY);

    //Ptr<ORB> orb = ORB::create(1000, 1.2f, )

    // feature detection, tracking
    vector<Point2f> points1, points2;        //vectors to store the coordinates of the feature points
	if (_state == 0 || (_prevFeatures.size() < 5)){
        Odometry::featureDetection(lframe_gray, _prevFeatures);        //detect features in img_1
    }

    Odometry::featureTracking(lframe_gray, cframe_gray, _prevFeatures, points2); //track those features to img_2

    if ((_prevFeatures.size() < 5) || (points2.size() < 5)) 
		return;
    //recovering the pose and the essential matrix
    Mat E, R, t, mask;
    //double focal = 718.8560;
    //cv::Point2d pp(607.1928, 185.2157);
    Mat cameraMatrix = Mat::eye(3, 3, CV_64F);
    E = findEssentialMat(points2, _prevFeatures, cameraMatrix, RANSAC, 0.999, 1.0, mask);
    recoverPose(E, points2, _prevFeatures, cameraMatrix, R, t, mask);

    if (_state == 0){
        _R_f = R.clone();
        _t_f = t.clone();
        _state++;
		_prevFrame = currentframe.clone();
        _prevFeatures = points2;
        return;
    }
    else {

        _t_f = _t_f + (_R_f * t);
        _R_f = R*_R_f;

        if (_prevFeatures.size() < 40)	{
            //cout << "Number of tracked features reduced to " << prevFeatures.size() << endl;
            //cout << "trigerring redection" << endl;
            featureDetection(lframe_gray, _prevFeatures);
            featureTracking(lframe_gray, cframe_gray, _prevFeatures, points2);

        }

		_prevFrame = currentframe.clone();
        _prevFeatures = points2;
        Odometry::DrawPath();
    }
}

void Odometry::DrawPath(){

    int x = int(_t_f.at<double>(0)) + 300;
    int y = int(_t_f.at<double>(1)) + 300;
    circle(_traj, Point(x, y), 1, CV_RGB(255, 0, 0), 2);

    rectangle(_traj, Point(10, 30), Point(550, 50), CV_RGB(0, 0, 0), CV_FILLED);
    //sprintf(text, "Coordinates: x = %02fm y = %02fm z = %02fm", _t_f.at<double>(0),
    //	_t_f.at<double>(1), _t_f.at<double>(2));

    //cv::Point textOrg(10, 50);
    //putText(_traj, text, textOrg, FONT_HERSHEY_PLAIN, 1, Scalar::all(255), 1, 8);

    imshow("Trajectory", _traj);
}

void Odometry::featureTracking(Mat img_1, Mat img_2, vector<Point2f>& points1, vector<Point2f>& points2)	{

    //this function automatically gets rid of points for which tracking fails
	vector<uchar> status;
    vector<float> err;
    Size winSize = Size(21, 21);
    TermCriteria termcrit = TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 30, 0.01);

    calcOpticalFlowPyrLK(img_1, img_2, points1, points2, status, err, winSize, 3, termcrit, 0, 0.001);

    //getting rid of points for which the KLT tracking failed or those who have gone outside the frame
    int indexCorrection = 0;
    for (int i = 0; i<status.size(); i++)
    {
        Point2f pt = points2.at(i - indexCorrection);
        if ((status.at(i) == 0) || (pt.x<0) || (pt.y<0))	{
            if ((pt.x<0) || (pt.y<0))	{
                status.at(i) = 0;
            }
            points1.erase(points1.begin() + (i - indexCorrection));
            points2.erase(points2.begin() + (i - indexCorrection));
            indexCorrection++;
        }
    }
}


void Odometry::featureDetection(Mat img_1, vector<Point2f>& points1)	{   //uses FAST as of now, modify parameters as necessary
    vector<KeyPoint> keypoints_1;
    int fast_threshold = 0;
    bool nonmaxSuppression = true;
    FAST(img_1, keypoints_1, fast_threshold, nonmaxSuppression);
    KeyPoint::convert(keypoints_1, points1, vector<int>());
}

void Odometry::SetFrame(Mat frame){
	_prevFrame = frame.clone();
}