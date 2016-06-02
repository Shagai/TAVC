#include "Tracking.h"


Tracking::Tracking(){

}


Tracking::~Tracking(){
	
}

// initialize the tracker
void Tracking::InitializeTracking(Mat frame, Rect roi){

    //this->roi = roi;
    _trackers.add("KCF", frame, roi);
}

int Tracking::UpdateTracking(Mat frame){

    // update the tracking result
    _trackers.update(frame);

	std::cout << "Número de Trackers: " << _trackers.objects.size() << std::endl;

	for (std::vector<Rect2d>::iterator it = _trackers.objects.begin();
		it != _trackers.objects.end();){
			
		if (it->x > 0 && it->x < 1280 && it->y > 0 && it->y < 720){
			cv::rectangle(frame, *it, Scalar(255, 0, 0), 2, 1);
			++it;
		}
		else it = _trackers.objects.erase(it);
	}

	return _trackers.objects.size();
}
