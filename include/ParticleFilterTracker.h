/**
 * Use onliner boosting classifier and particle filter.
 * Initialize a target, and track it.
 * 
 * This tracker use basic particle model,
 * with only x and y coordinates.
 * Other movement model can be used, by overwrite the propagate and init methods.
 *
 * @author Zhengrong Wang
 */

#ifndef PARTICLEFILTER_TRACKER_CLASS
#define PARTICLEFILTER_TRACKER_CLASS

#include "Tracker.h"
#include "StrongClassifier.h"
#include "ParticleFilter.h"
#include "SingleSampler.h"

#define PTPRINTF(...) printf(__VA_ARGS__)
// #define PTPRINTF(...)

class ParticleFilterTracker : public Tracker {
public:
	/**
	 * Construct a tracker.
	 * Initialize the particles randomly.
	 */
	ParticleFilterTracker(StrongClassifier *classifier, IntegralImage *intImage,
		ParticleFilter *particleFilter, SingleSampler *sampler);

	~ParticleFilterTracker();

	virtual void Track(cv::VideoCapture &in, cv::VideoWriter &out);

protected:
	StrongClassifier *classifier;
	IntegralImage *intImage;
	ParticleFilter *particleFilter;
	SingleSampler *sampler;
};

#endif