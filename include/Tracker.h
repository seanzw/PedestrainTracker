/***************************************************
 Tracker interface.
 Author: Zhengrong Wang.
 ***************************************************/

#ifndef TRACKER_HEADER
#define TRACKER_HEADER

#include "GlobalHeader.h"

class Tracker {
public:
	Tracker();
	virtual ~Tracker();

	virtual void Track(cv::VideoCapture &in, cv::VideoWriter &out) = 0;
};


#endif