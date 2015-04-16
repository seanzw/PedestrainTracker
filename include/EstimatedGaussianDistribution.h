/********************************************
 EstimatedGaussianDistribution class.
 Estimate a N dimension Gaussian distribution with Kalman filter.
 ********************************************/

#ifndef ESTIMATED_GAUSSIAN_HEADER
#define ESTIMATED_GAUSSIAN_HEADER

#include "GlobalHeader.h"

template<int N> class EstimatedGaussianDistribution {
public:
	EstimatedGaussianDistribution();
	EstimatedGaussianDistribution(float pMean, float pSigma, float rMean, float rSigma);

	~EstimatedGaussianDistribution();

	// Use Kalman filter to update the Gaussian distribution.
	void Update(float *value);

	// Get and set the data.
	float GetMean() const { return m_mean; }
	float GetSigma() const { return m_sigma; }
	void SetValues(float *mean, float *sigma);

private:
	float m_mean[N], m_sigma[N];
	float m_pMean, m_pSigma;
	float m_rMean, m_rSigma;

};

#endif