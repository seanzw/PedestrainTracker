/**
 * Our final weapon!
 *
 * @author Zhengrong Wang.
 */

#ifndef MULTITRACKER_HEADER
#define MULTITRACKER_HEADER

#define MT_DEBUG
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
	 * @param size		size of a frame
	 * @patam opts		all the options
	 */
	MultiTracker(ImageDetector *detector, const Size &imgSize, const Options &opts);

	~MultiTracker();

	void Track(cv::VideoCapture &in, cv::VideoWriter &out, const cv::Mat &bkg = defaultBackground);

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

	// Match matrix.
	MatchMatrix *matches;

	// MultiSampler.
	MultiSampler *sampler;

	// Threshold for matching.
	const float matchThre;
};

#endif