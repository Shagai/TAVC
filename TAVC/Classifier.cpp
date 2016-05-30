#include "Classifier.h"

Classifier::Classifier(String fname){

	phone1_cascade.load(fname);
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
