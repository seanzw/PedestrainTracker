/**
 * This class use N-D Gaussian distribution for pos 
 * and neg data set.
 * Given a feature, it returns whether it's positive 
 * or negative.
 * @author Zhengrong Wang.
 */

#ifndef CLASSIFIER_THRESHOLD_HEADER
#define CLASSIFIER_THRESHOLD_HEADER

#include "EstimatedGaussianDistribution.h"
#include "Feature.h"

template<int N> class ClassifierThreshold {
public:
	ClassifierThreshold();
	~ClassifierThreshold();

	/**
	 * Use Kalman filter to update the pos and neg cluster.
	 * @param feature: the feature extracted, should be N dimension.
	 * @param target: positive > 0, negative <= 0.
	 */
	void Update(const Feature &feature, int target);

	/**
	 * Evaluate the feature.
	 * Use the simple Euclidean distance to pos and neg cluster.
	 * @param feature: the feature extracted, should be N dimension.
	 * @return int: 1 for positive, -1 for negative.
	 */
	int Classify(const Feature &feature);

	void Reset();

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

template<int N> ClassifierThreshold<N>::ClassifierThreshold() {
	posSamples = new EstimatedGaussianDistribution<N>();
	negSamples = new EstimatedGaussianDistribution<N>();
}

template<int N> ClassifierThreshold<N>::~ClassifierThreshold() {
	if (posSamples != NULL)
		delete posSamples;
	if (negSamples != NULL)
		delete negSamples;
}

template<int N> void ClassifierThreshold<N>::Update(const Feature &feature, int target) {
	if (target > 0)
		posSamples->Update(feature.data);
	else
		negSamples->Update(feature.data);
}

template<int N> int ClassifierThreshold<N>::Classify(const Feature &feature) {
	float dPos = SquareDistance(feature.data, posSamples->mean, N);
	float dNeg = SquareDistance(feature.data, negSamples->mean, N);
	return dPos < dNeg ? 1 : -1;
}

template<int N> void *ClassifierThreshold<N>::GetDistribution(int target) const {
	if (target > 0)
		return posSamples;
	else
		return negSamples;
}

#endif