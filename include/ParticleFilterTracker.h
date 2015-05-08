/**
 * Use onliner boosting classifier and particle filter.
 * Given a target, and track it.
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