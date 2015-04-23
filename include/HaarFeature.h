/********************************************
 Haar Feature class.
 Extract a Haar like feature from the image.
 Author: Zhengrong Wang
 ********************************************/

#ifndef HAAR_FEATURE_HEADER
#define HAAR_FEATURE_HEADER

#include "EstimatedGaussianDistribution.h"
#include "IntegralImage.h"
#include "Feature.h"

class HaarFeature {
public:
	HaarFeature(const Size &patchSize);
	virtual ~HaarFeature();

	// void getInitialDistribution(EstimatedGaussianDistribution * distribution);

	bool Extract(IntegralImage *intImage, const Rect &roi, Feature *feature);

	float GetResponse() { return response; }

	int GetNumAreas() { return numAreas; }

	int *GetWeights() { return weights; }

private:
	int type;
	int numAreas;
	int *weights;
	float initMean;
	float initSigma;

	// Generate a random Haar feature extractor.
	void GenerateRandomFeature(const Size &imageSize);
	
	// areas within the patch to compute the feature.
	Rect *areas;
	
	// size of patch used in training.
	Size initSize;
	
	// current size under investigation.
	Size curSize;

	// scale factor in vertical direction.
	float scaledHeight;

	// scale factor in horizontal direction.
	float scaledWidth;

	// areas after scaling.
	Rect *scaledAreas;

	// weights after scaling.
	float *scaledWeights;

	// The result feature from last extraction.
	float response;

	// A CDF for six types.
	static const float cdf[5];

	// Minimum size.
	static const Size minimumSize;
	static const int minimumArea;
};

#endif