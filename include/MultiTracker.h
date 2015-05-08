/**
 * Our final weapon!
 *
 * @author Zhengrong Wang.
 */

#ifndef MULTITRACKER_HEADER
#define MULTITRACKER_HEADER

#include "Tracker.h"
#include "RGIIntegralImage.h"
#include "HoGIntegralImage.h"
#include "TargetsFreeList.h"
#include "ImageDetector.h"

class MultiTracker : public Tracker {
public:


private:

	// Integral Images.
	RGIIntegralImage *rgiIntImage;
	HoGIntegralImage *hogIntImage;

	// Detector.
	ImageDetector *detector;

	// Target pool.


};

#endif