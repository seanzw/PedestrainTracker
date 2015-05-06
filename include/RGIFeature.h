/**
 * RGI feature.
 * @author Zhengrong Wang
 */

#ifndef RGIFEATURE_HEADER
#define RGIFEATURE_HEADER

#include "EstimatedGaussianDistribution.h"
#include "IntegralImage.h"
#include "Feature.h"

#define NUM_RGI_BINS 9

class RGIFeature {
public:
	RGIFeature(const Size &patchSize);
	virtual ~RGIFeature();

	void GetInitialDistribution(EstimatedGaussianDistribution<NUM_RGI_BINS> *distribution) const;
	bool Extract(const IntegralImage *intImage, const Rect &roi, Feature *feature);

private:

	Rect patch, scaledPatch;

	// size of initial patch.
	Size initSize;

	// current size under investigation.
	Size curSize;

	// scale factor in vertical direction.
	float scaledHeight;

	// scale factor in horizontal direction.
	float scaledWidth;

	void GeneratreRandomFeature(const Size &imgSize);

	inline void NormalizeHistogram(float *histogram, int N) const {
		float sum = 0.0f;
		for (int i = 0; i < N; i++) {
			
			// Some tests.
			assert(histogram[i] >= 0.0f);

			sum += histogram[i];
		}
		float invSum = 1.0f / sum;
		for (int i = 0; i < N; i++) {
			histogram[i] *= invSum;
		}
	}

	// Minimum size.
	static const Size minimumSize;
	static const int minimumArea;

	const static float initMean[NUM_RGI_BINS];
	const static float initSigma[NUM_RGI_BINS];
};

#endif