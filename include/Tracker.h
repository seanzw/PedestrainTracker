/*******************************************************************************
 Tracker class.
 Use onliner boosting classifier and particle filter.
 Initialize a target, and track it.

 This tracker use basic particle model,
 with only x and y coordinates.
 Other movement model can be used, by overwrite the propagate and init methods.

 Author: Zhengrong Wang.
 ******************************************************************************/

#ifndef TRACKER_CLASS
#define TRACKER_CLASS

#include "GlobalHeader.h"
#include "StrongClassifierDirectSelect.h"

class Tracker {
public:
	// Construct a tracker.
	// Initialize the particles randomly.
	// @param featureExt: the feature extractor used here.
	// @param classifier: the classifier used here to detect.
	// @param n: the number of particles.
	Tracker(FeatureExtractor *featureExt, StrongClassifier *classifier,
		int n = 100);

	// Destructor.
	~Tracker();

	// Propagate the particles.
	virtual void Propagate();

	// Observe.
	void Observe(const cv::Mat &img);

	// Draw the particles.
	void DrawParticles(cv::Mat &img, const cv::Scalar &color) const;

	// Draw the target;
	void DrawTarget(cv::Mat &img, const cv::Scalar &color) const;

private:
	// The size of the image.
	const int imgW, imgH;

	// Where the target is.
	int centerX, centerY;

	// The size of the target.
	int width, height;

	// The sizeof particles. Here it should be 2. Its structure like:
	// struct Particle {
	//		int x;
	//		int y;
	// }
	const static int sizeParticle;
	int *particles;					// The particles.
	const int numParticles;			// Number of particles.
	float *confidence;				// The confidence map of particles.

	// Random generator.
	std::normal_distribution<float> gaussian;

	// Uniform generator.
	static std::default_random_engine generator;

	// The online boosting classifier.
	StrongClassifierDirectSelect *classifier;

	// Feature extractor.
	FeatureExtractor *featureExt;

	// A feature instance to store the result from extractor.
	Feature feature;

	// Initialize the particles.
	void InitParticles();
};

#endif