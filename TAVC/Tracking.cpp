#include "Tracking.h"
#include <chrono>

Tracking::Tracking(){

}


Tracking::~Tracking(){
	
}

// initialize the tracker
void Tracking::InitializeTracking(Mat frame, Rect roi){

	std::cout << "Número de Trackers: " << _trackers.objects.size() << std::endl;
	if (_trackers.objects.size() == 0) _trackers.add("MIL", frame, roi);
	else {
		int count = 0;
		for (std::vector<Rect2d>::iterator it = _trackers.objects.begin();
			it != _trackers.objects.end() && abs(roi.x - it->x) < 400; ++it){
			count++;
		}

		if (count < _trackers.objects.size()) {
			if (roi.x > 0 && (roi.x + roi.width) < 1280 && roi.y > 0 &&
				(roi.y + roi.height) < 720){
				_trackers.add("MIL", frame, roi);
			}
		}
	}
}

int Tracking::UpdateTracking(Mat frame){

    // update the tracking result
	
	if (_trackers.objects.size() > 0) {
		//std::chrono::high_resolution_clock::time_point t4 = std::chrono::high_resolution_clock::now();
		_trackers.update(frame);
		//std::chrono::high_resolution_clock::time_point t5 = std::chrono::high_resolution_clock::now();
		//if (std::chrono::duration_cast<std::chrono::milliseconds>(t5 - t4).count() > 100) _trackers.objects.erase(_trackers.objects.begin());
	}
   
	//std::cout << "Número de Trackers: " << _trackers.objects.size() << std::endl;

	for (std::vector<Rect2d>::iterator it = _trackers.objects.begin();
		it != _trackers.objects.end();){
			
		std::cout << "x: " << it->x << " y: " << it->y << std::endl;
		std::cout << "width: " << it->width << " height: " << it->height << std::endl;
		if (it->x > 0 && (it->x) < 1280 && it->y > 0 && (it->y) < 720){
			cv::rectangle(frame, *it, Scalar(0, 255, 0), 2, 1);
			++it;
		}
		else it = _trackers.objects.erase(it);
	}

	return _trackers.objects.size();
}
