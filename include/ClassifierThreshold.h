/*************************************************
 ClassifierThreshold class.
 This class use N-D Gaussian distribution for pos 
 and neg data set.
 Given a feature, it returns whether it's positive 
 or negative.
 Author: Zhengrong Wang.
 ************************************************/

#ifndef CLASSIFIER_THRESHOLD_HEADER
#define CLASSIFIER_THRESHOLD_HEADER

#include "EstimatedGaussianDistribution.h"
#include "Feature.h"

template<int N> class ClassifierThreshold {
public:
	ClassifierThreshold();
	~ClassifierThreshold();

	// Use Kalman filter to update the pos and neg cluster.
	// @param feature: the feature extracted, should be N dimension.
	// @param target: positive > 0, negative <= 0.
	// @return void.
	void Update(const Feature &feature, int target);

	// Evaluate the feature.
	// Use the simple Euclidean distance to pos and neg cluster.
	// @param feature: the feature extracted, should be N dimension.
	// @return int: 1 for positive, -1 for negative.
	int Evaluate(const Feature &feature) const;

	void Reset();

	// Get a pointer to the distribution.
	void *GetDistribution(int target) const;

private:

	EstimatedGaussianDistribution<N>* posSamples;
	EstimatedGaussianDistribution<N>* negSamples;

	inline float SquareDistance(const float *a, const float *b, int N) {
		float d = 0.0f;
		for (int i = 0; i < N; i++) {
			d += (a[i] - b[i]) * (a[i] - b[i]);
		}
		return d;
	}
};

#endif