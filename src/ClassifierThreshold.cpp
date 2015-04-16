#include "ClassifierThreshold.h"

template<int N> ClassifierThreshold<N>::ClassifierThreshold() {
	m_posSamples = new EstimatedGaussianDistribution<N>();
	m_negSamples = new EstimatedGaussianDistribution<N>();
	m_threshold = 0.0f;
	m_parity = 0;
}

template<int N> ClassifierThreshold<N>::~ClassifierThreshold() {
	delete m_posSamples;
	delete m_negSamples;
}

template<int N> void ClassifierThreshold<N>::Update(float *feature, int target) {
	if (target == 1)
		m_posSamples->Update(feature);
	else
		m_negSamples->Update(feature);
}

template<int N> bool ClassifierThreshold<N>::Eval(float *feature) const {
	float dPos = SquareDistance(feature, m_posSamples->m_mean, N);
	float dNeg = SquareDistance(feature, m_negSamples->m_mean, N);
	return dPos < dNeg;
}

template<int N> void *ClassifierThreshold<N>::GetDistribution(int target) const {
	if (target == 1)
		return m_posSamples;
	else
		return m_negSamples;
}

