/**
 * Estimate a N dimension Gaussian distribution with Kalman filter.
 */

#ifndef ESTIMATED_GAUSSIAN_HEADER
#define ESTIMATED_GAUSSIAN_HEADER

#include "GlobalHeader.h"

template<int N> class EstimatedGaussianDistribution {
public:
	EstimatedGaussianDistribution(float pm = 1000.0f, float ps = 1000.0f, float rm = 0.01f, float rs = 0.01f);

	~EstimatedGaussianDistribution();

	/**
	 * Use Kalman filter to update the Gaussian distribution.
	 */
	void Update(const float *value);

	void SetValues(const float *m, const float *s);

	float mean[N], sigma[N];
	float pMean, pSigma;
	float rMean, rSigma;

};

template<int N> EstimatedGaussianDistribution<N>::EstimatedGaussianDistribution(
	float pm, float ps, float rm, float rs)
	: pMean(pm), pSigma(ps), rMean(rm), rSigma(rs) {
	for (int i = 0; i < N; i++) {
		mean[i] = 0.0f;
		sigma[i] = 1.0f;
	}
}

template<int N> EstimatedGaussianDistribution<N>::~EstimatedGaussianDistribution() {

}

template<int N> void EstimatedGaussianDistribution<N>::Update(const float *value) {
	// Use Kalman filter to update the Gaussian distribution.
	// K: Kalman gain.
	float K;
	float minFactor = 0.001f;

	// Update the mean.
	K = pMean / (pMean + rMean);
	if (K < minFactor)
		K = minFactor;

	for (int i = 0; i < N; i++) {
		mean[i] += K * (value[i] - mean[i]);
	}
	pMean = pMean * rMean / (pMean + rMean);

	// Update the sigma.
	K = pSigma / (pSigma + rSigma);
	if (K < minFactor)
		K = minFactor;

	for (int i = 0; i < N; i++) {
		float sigma2 = K * (mean[i] - value[i]) * (mean[i] - value[i]) +
			(1.0f - K) * sigma[i] * sigma[i];
		sigma[i] = sqrtf(sigma2);
		if (sigma[i] < 1.0f)
			sigma[i] = 1.0f;
	}

	pSigma = pSigma * rSigma / (pSigma + rSigma);
}

template<int N> void EstimatedGaussianDistribution<N>::SetValues(const float *m, const float *s) {
	memcpy(mean, m, sizeof(float) * N);
	memcpy(sigma, s, sizeof(float) * N);
}

#endif