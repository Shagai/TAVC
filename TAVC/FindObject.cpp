#include "FindObject.h"


FindObject::FindObject(String fileName, int minHessian)
{
	_img_object = imread(fileName, IMREAD_GRAYSCALE);
	resize(_img_object, _img_object, Size(_img_object.cols / 2, _img_object.rows / 2));
	_detector = SURF::create(minHessian);
	_detector->detectAndCompute(_img_object, Mat(), _keypoints_object, 
								_descriptors_object);
}


FindObject::~FindObject()
{
}

void FindObject::Find(Mat frame){

	Mat img_scene;
	resize(frame, frame, Size(frame.cols / 2, frame.rows / 2));
	cvtColor(frame, img_scene, COLOR_BGR2GRAY);

	std::vector<KeyPoint> keypoints_scene;
	Mat descriptors_scene;

	_detector->detectAndCompute(img_scene, Mat(), keypoints_scene, descriptors_scene);

	std::cout << "Keypoints: " << keypoints_scene.size() << std::endl;
	if (keypoints_scene.size() < 15)return;

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

	if (good_matches.size() < 5) return;

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

	if (H.rows == 0) return;

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

	std::cout << "width: " << mark.width << " height: " << mark.height << std::endl;

	if (mark.width < 30 || mark.height < 30 || 
		mark.width > 1.8 * mark.height || mark.height > 1.8 * mark.width) return;
	else cv::rectangle(frame, mark, Scalar(255));

	//-- Show detected matches
	imshow("Good Matches & Object detection", frame);
}
