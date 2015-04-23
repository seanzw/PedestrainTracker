/********************************************
 EstimatedGaussianDistribution class.
 Estimate a N dimension Gaussian distribution with Kalman filter.
 ********************************************/

#ifndef ESTIMATED_GAUSSIAN_HEADER
#define ESTIMATED_GAUSSIAN_HEADER

#include "GlobalHeader.h"

template<int N> class EstimatedGaussianDistribution {
public:
	EstimatedGaussianDistribution(float pm = 1000.0f, float ps = 1000.0f, float rm = 0.01f, float rs = 0.01f);

	~EstimatedGaussianDistribution();

	// Use Kalman filter to update the Gaussian distribution.
	void Update(const float *value);

	void SetValues(const float *m, const float *s);

private:
	float mean[N], sigma[N];
	float pMean, pSigma;
	float rMean, rSigma;

};

#endif