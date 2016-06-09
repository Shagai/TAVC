#include "Classifier.h"

Classifier::Classifier(String fname){
	_cascade.load(fname);
}

Classifier::Classifier(String fileName, int minHessian){
	_img_object = imread(fileName, IMREAD_GRAYSCALE);
	resize(_img_object, _img_object, Size(_img_object.cols / 2, _img_object.rows / 2));
	_detector = SURF::create(minHessian);
	_detector->detectAndCompute(_img_object, Mat(), _keypoints_object,
		_descriptors_object);
}

Classifier::~Classifier(){
}

void Classifier::Init(String fname){
	
}

std::vector<Rect> Classifier::ImageDetection(Mat frame){

	std::vector<Rect> marks;
	Mat frame_gray;

	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	//-- Detect Obeject
	_cascade.detectMultiScale(frame_gray, marks, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

	return marks;
}

void Classifier::DrawMarks(std::vector<Rect> marks, Mat frame){

	for (int i = 0; i < marks.size(); ++i)
		cv::rectangle(frame, marks[i], Scalar(255));
}

void Classifier::CheckDetection(std::vector<Rect> &marks, Mat frame, Scalar minRange, Scalar maxRange){

	for (std::vector<Rect>::iterator it = marks.begin(); it != marks.end();){

		Mat imgHSV;

		double areaTotal = -1;
		int contourPoints = -1;

		cv::cvtColor(frame(*it), imgHSV, CV_BGR2HSV);
		cv::inRange(imgHSV, minRange, maxRange, imgHSV);
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

		if (areaTotal / it->area() < 0.1) it = marks.erase(it);
		else ++it;
	}
	
	for (std::vector<Rect>::iterator it1 = marks.begin(); it1 != marks.end();){

		for (std::vector<Rect>::iterator it2 = (marks.begin() + 1); it2 != marks.end();){

			if (abs(it1->x - it2->x) < 100) it2 = marks.erase(it2);
			else ++it2;
		}
		
		if (marks.size() == 1) break;
		else ++it1;
	}
}

bool Classifier::CheckDetection(Rect mark, Mat frame, Scalar minRange, Scalar maxRange){

	Mat imgHSV;

	double areaTotal = -1;
	int contourPoints = -1;

	cv::cvtColor(frame(mark), imgHSV, CV_BGR2HSV);
	cv::inRange(imgHSV, minRange, maxRange, imgHSV);
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

		std::cout << "Area Total: " << areaTotal << std::endl;
		if (areaTotal/mark.area() < 0.3) return false;
		else return true;
	}
}


Rect Classifier::FeatureDetection(Mat frame){

	Rect rect(0,0,0,0);
	Mat img_scene;
	resize(frame, img_scene, Size(frame.cols / 2, frame.rows / 2));
	cvtColor(img_scene, img_scene, COLOR_BGR2GRAY);

	std::vector<KeyPoint> keypoints_scene;
	Mat descriptors_scene;

	_detector->detectAndCompute(img_scene, Mat(), keypoints_scene, descriptors_scene);

	//std::cout << "Keypoints: " << keypoints_scene.size() << std::endl;
	if (keypoints_scene.size() < 15)return rect;

	FlannBasedMatcher matcher;
	std::vector< DMatch > matches;
	matcher.match(_descriptors_object, descriptors_scene, matches);

	double max_dist = 0; double min_dist = 100;
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
		if (matches[i].distance < 1.5 * min_dist)
		{
			good_matches.push_back(matches[i]);
		}
	}

	if (good_matches.size() < 5) return rect;

	//-- Localize the object
	std::vector<Point2f> obj;
	std::vector<Point2f> scene;
	for (size_t i = 0; i < good_matches.size(); i++)
	{
		//-- Get the keypoints from the good matches
		obj.push_back(_keypoints_object[good_matches[i].queryIdx].pt);
		scene.push_back(keypoints_scene[good_matches[i].trainIdx].pt);
	}

	Mat H = findHomography(obj, scene, RANSAC);

	if (H.rows == 0) return rect;

	//-- Get the corners from the image_1 ( the object to be "detected" )
	std::vector<Point2f> obj_corners(4);
	obj_corners[0] = cvPoint(0, 0);
	obj_corners[1] = cvPoint(_img_object.cols, 0);
	obj_corners[2] = cvPoint(_img_object.cols, _img_object.rows);
	obj_corners[3] = cvPoint(0, _img_object.rows);
	std::vector<Point2f> scene_corners(4);

	perspectiveTransform(obj_corners, scene_corners, H);

	//-- Draw lines between the corners (the mapped object in the scene - image_2 )
	Rect mark(scene_corners[0].x, scene_corners[0].y, norm(scene_corners[0] - scene_corners[1]), norm(scene_corners[0] - scene_corners[3]));

	//std::cout << "width: " << mark.width << " height: " << mark.height << std::endl;

	if (/*mark.width < 30 || mark.height < 30 ||*/
		mark.width > 6 * mark.height || mark.height > 6 * mark.width) return rect;
	if (mark.x * 2 < 0 || mark.y * 2 < 0 || 
		mark.x * 2 + mark.width * 2 > 1280 ||
		mark.y * 2 + mark.height * 2 > 720) return rect;
	
	//cv::rectangle(frame, Rect(mark.x*2, mark.y*2, mark.width*2, mark.height * 2), Scalar(255));
	//cv::imshow("test", frame);

	return Rect(mark.x * 2, mark.y * 2, mark.width * 2, mark.height * 2);
}
