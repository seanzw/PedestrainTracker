#include "EstimatedGaussianDistribution.h"

template<int N> EstimatedGaussianDistribution<N>::EstimatedGaussianDistribution() {
	m_mean = 0.0f;
	m_sigma = 1.0f;
	m_pMean = 1000.0f;
	m_rMean = 0.01f;
	m_pSigma = 1000.0f;
	m_rSigma = 0.01f;
}

template<int N> EstimatedGaussianDistribution<N>::EstimatedGaussianDistribution(
	float pMean, float pSigma, float rMean, float rSigma) {
	m_mean = 0.0f;
	m_sigma = 1.0f;
	m_pMean = pMean;
	m_rMean = rMean;
	m_pSigma = pSigma;
	m_rSigma = rSigma;
}

template<int N> EstimatedGaussianDistribution<N>::~EstimatedGaussianDistribution() {

}

template<int N> void EstimatedGaussianDistribution<N>::Update(float *value) {
	// Use Kalman filter to update the Gaussian distribution.
	// K: Kalman gain.
	float K;
	float minFactor = 0.001f;

	// Update the mean.
	K = m_pMean / (m_pMean + m_rMean);
	if (K < minFactor)
		K = minFactor;

	for (int i = 0; i < N; i++) {
		m_mean[i] += K * (value[i] - m_mean[i]);
	}
	m_pMean = m_pMean * m_rMean / (m_pMean + m_rMean);

	// Update the sigma.
	K = m_pSigma / (m_pSigma + m_rSigma);
	if (K < minFactor)
		K = minFactor;

	for (int i = 0; i < N; i++) {
		float sigma2 = K * (m_mean[i] - value[i]) * (m_mean[i] - value[i]) +
			(1.0f - K) * m_sigma[i] * m_sigma[i];
		m_sigma[i] = sqrtf(sigma2);
		if (m_sigma[i] < 1.0f)
			m_sigma[i] = 1.0f;
	}
	
	m_pSigma = m_pSigma * m_rSigma / (m_pSigma + m_rSigma);
}

template<int N> void EstimatedGaussianDistribution<N>::SetValues(float *mean, float *sigma) {
	memcpy(m_mean, mean, sizeof(float) * N);
	memcpy(m_sigma, sigma, sizeof(float) * N);
}