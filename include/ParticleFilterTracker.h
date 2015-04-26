/*******************************************************************************
 Tracker class.
 Use onliner boosting classifier and particle filter.
 Initialize a target, and track it.

 This tracker use basic particle model,
 with only x and y coordinates.
 Other movement model can be used, by overwrite the propagate and init methods.

 Author: Zhengrong Wang.
 ******************************************************************************/

#ifndef PARTICLEFILTER_TRACKER_CLASS
#define PARTICLEFILTER_TRACKER_CLASS

#include "Tracker.h"
#include "StrongClassifier.h"
#include "ParticleFilter.h"

class ParticleFilterTracker : public Tracker {
public:
	// Construct a tracker.
	// Initialize the particles randomly.
	// @param classifier: the strong classifier used in observation.
	// @param intImage: the integral image.
	// @param particleFilter: the particle filter.
	ParticleFilterTracker(StrongClassifier *classifier, IntegralImage *intImage,
		ParticleFilter *particleFilter);

	// Destructor.
	~ParticleFilterTracker();

	// Track.
	virtual void Track(cv::VideoCapture &in, cv::VideoWriter &out);

protected:
	StrongClassifier *classifier;
	IntegralImage *intImage;
	ParticleFilter *particleFilter;
};

#endif