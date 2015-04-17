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

template<int N> class ClassifierThreshold {
public:
	ClassifierThreshold();
	~ClassifierThreshold();

	void Update(float *feature, bool target);
	bool Eval(float *feature) const;
	float GetValue(float *feature) const;

	void Reset();

	void *GetDistribution(bool target) const;

private:

	EstimatedGaussianDistribution<N>* m_posSamples;
	EstimatedGaussianDistribution<N>* m_negSamples;

	inline float SquareDistance(float *a, float *b, int N) {
		float d = 0.0f;
		for (int i = 0; i < N; i++) {
			d += (a[i] - b[i]) * (a[i] - b[i]);
		}
		return d;
	}
};

#endif