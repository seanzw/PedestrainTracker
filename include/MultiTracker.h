/**
 * Our final weapon!
 *
 * @author Zhengrong Wang.
 */

#ifndef MULTITRACKER_HEADER
#define MULTITRACKER_HEADER

#define MTPRINTF(...) printf(__VA_ARGS__)

#include "Tracker.h"
#include "RGIIntegralImage.h"
#include "HoGIntegralImage.h"
#include "TargetsFreeList.h"
#include "ImageDetector.h"

class MultiTracker : public Tracker {
public:

	/**
	 * @param detector	image detector
	 * @param capacity	maximum number of targets
	 * @param size		size of a frame
	 */
	MultiTracker(ImageDetector *detector, int capacity, const Size &imgSize);

	~MultiTracker();

	void Track(cv::VideoCapture &in, cv::VideoWriter &out);

private:

	// Integral Images.
	RGIIntegralImage *rgiIntImage;
	HoGIntegralImage *hogIntImage;

	// Detector.
	ImageDetector *detector;

	// Target pool.
	TargetsFreeList *targets;

	// Size of the frame.
	const Size imgSize;
};

#endif