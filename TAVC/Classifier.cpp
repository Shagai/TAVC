#include "Classifier.h"

Classifier::Classifier(String fname){

	phone1_cascade.load(fname);
	_object = imread("logo1.png", IMREAD_GRAYSCALE);
	_detector = xfeatures2d::SURF::create();
	_detector->setHessianThreshold(8000);
	_detector->detectAndCompute(_object, Mat(), _keypoints_object, _descriptors_object);
}

Classifier::~Classifier(){
	
}

std::vector<Rect> Classifier::ImageDetection(Mat frame){

	std::vector<Rect> marks;
	Mat frame_gray;

	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	//-- Detect Obeject
	phone1_cascade.detectMultiScale(frame_gray, marks, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

	return marks;
}

void Classifier::DrawMarks(std::vector<Rect> marks, Mat frame){

	for (int i = 0; i < marks.size(); ++i)
		cv::rectangle(frame, marks[i], Scalar(255));
}

void Classifier::CheckDetection(std::vector<Rect> &marks, Mat frame){

	for (std::vector<Rect>::iterator it = marks.begin(); it != marks.end();){

		Mat imgHSV;

		double areaTotal = -1;
		int contourPoints = -1;

		cv::cvtColor(frame(*it), imgHSV, CV_BGR2HSV);
		cv::inRange(imgHSV, Scalar(95, 125, 85), Scalar(115, 255, 255), imgHSV);
		//cv::imshow("Tasda", imgHSV);

		if (imgHSV.cols != 0){
			std::vector<std::vector<Point> > contours;
			std::vector<Vec4i> hierarchy;
			threshold(imgHSV, imgHSV, 127, 255, THRESH_BINARY);
			findContours(imgHSV, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);

			areaTotal = 0;
			contourPoints = 0;

			for (int i = 0; i < contours.size(); i++){
				double area = contourArea(contours[i]);
				if (area > 0){
					areaTotal += area;
					contourPoints += contours[i].size();
				}
			}

			// Area de 2000 es bien
			//std::cout << "Area Total: " << areaTotal << std::endl;
			//std::cout << "Numero Puntos: " << contourPoints << std::endl;
		}

		if (areaTotal < 2000) it = marks.erase(it);
		else ++it;
	}

	_prevNumMarks = marks.size();
}

void Classifier::FeatureDetection(std::vector<Rect> marks, Mat frame){

	//-- Step 1: Detect the keypoints and extract descriptors using ORB
	Mat img_scene;
	std::vector<KeyPoint> keypoints_scene;
	Mat descriptors_scene;
	//img_scene = frame(marks[0]).clone();
	cv::cvtColor(frame(marks[0]), img_scene, CV_BGR2GRAY);

	_detector->detectAndCompute(img_scene, Mat(), keypoints_scene, descriptors_scene);

	if (keypoints_scene.size() > 0){
		//-- Step 2: Matching descriptor vectors using FLANN matcher
		FlannBasedMatcher matcher;
		std::vector< DMatch > matches;
		matcher.match(_descriptors_object, descriptors_scene, matches);

		double max_dist = 0; double min_dist = 20;

		//-- Quick calculation of max and min distances between keypoints
		for (int i = 0; i < _descriptors_object.rows; i++)
		{
			double dist = matches[i].distance;
			if (dist < min_dist) min_dist = dist;
			if (dist > max_dist) max_dist = dist;
		}

		//-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
		std::vector< DMatch > good_matches;
		for (int i = 0; i < _descriptors_object.rows; i++)
		{
			if (matches[i].distance < 3 * min_dist)
			{
				good_matches.push_back(matches[i]);
			}
		}

		Mat img_matches;
		std::cout << good_matches.size() << std::endl;
		drawMatches(_object, _keypoints_object, img_scene, keypoints_scene,
			good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
			std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
		//-- Show detected matches
		imshow("Good Matches", img_matches);
		int i = 0;
	}

	cv::imshow("Test", img_scene);
}
