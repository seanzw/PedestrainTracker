/***********************************************
 Detector base class.
 There are basically two kinds of detectors.
 ImageDetector: detects pedestrains in an image.
 VideoDetector: detects pedestrains in a video.
 ***********************************************/
/*
#ifndef DETECTOR_HEADER
#define DETECTOR_HEADER

#include "FeatureExtractor.h"
#include "Classifier.h"


// Store the options for detectors.
struct Options {
	feat scaleMin;
	feat scaleMax;
	feat scaleStep;
	int slideStep;
	int evidence;		// Number of neighboring detection to make sure.
	int modelHeight;
	int modelWidth;
};

// Use some specific feature and classifier to detect pedestrain.
// Apply this directly on the whole image.
class ImageDetector {
public:
	ImageDetector(FeatureExtractor &fe, Classifier &c, Options &op);
	
	// Do the detection.
	void Detect(const cv::Mat &img, const cv::Mat &imgGray) const;


protected:
	FeatureExtractor &featureExt;
	Classifier &classifier;
	feat scaleMin;
	feat scaleMax;
	feat scaleStep;
	int slideStep;
	int evidence;
	int modelHeight;
	int modelWidth;
};

#endif
*/