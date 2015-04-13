/*************************************************
 Video Detector class.
 Author: Zhengrong Wong.
 *************************************************/

#ifndef VIDEO_DETECTOR_HEADER
#define VIDEO_DETECTOR_HEADER

#include "ImageDetector.h"

// Switch this to turn on/off debug out put.
#define VDPRINTF(...) printf(__VA_ARGS__)
// #define VDPRINTF(...) 

class VideoDetector {
public:
	// Constructor.
	VideoDetector(ImageDetector *id, const Options &opt);

	// Detects.
	void Detect(cv::VideoCapture &in, cv::VideoWriter &out,
		const cv::Mat &bkg = defaultBackground);

protected:
	ImageDetector *imgDetector;

};

#endif