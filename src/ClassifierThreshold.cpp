#include "ClassifierThreshold.h"

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

template<int N> int ClassifierThreshold<N>::Classify(const Feature &feature) const {
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

