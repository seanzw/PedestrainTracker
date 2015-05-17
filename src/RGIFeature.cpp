#include "RGIFeature.h"

#define RGI_SIGMA 73.9f

// Define the minimum size and area.
const Size RGIFeature::minimumSize(3, 3);
const int RGIFeature::minimumArea = 9;

const float RGIFeature::initMean[NUM_RGI_BINS] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
const float RGIFeature::initSigma[NUM_RGI_BINS] = {
	RGI_SIGMA,
	RGI_SIGMA,
	RGI_SIGMA,
	RGI_SIGMA,
	RGI_SIGMA,
	RGI_SIGMA,
	RGI_SIGMA,
	RGI_SIGMA,
	RGI_SIGMA
};

RGIFeature::RGIFeature(const Size &patchSize) {
	try {
		GeneratreRandomFeature(patchSize);
	}
	catch (...) {
		
	}
}

RGIFeature::~RGIFeature() {

}

void RGIFeature::Reset(const Size &patchSize) {
	try {
		GeneratreRandomFeature(patchSize);
	}
	catch (...) {

	}
}

void RGIFeature::GeneratreRandomFeature(const Size &imgSize) {

	bool valid = false;

	while (!valid) {

		// Choose upper-left corner.
		patch.left = rand() % imgSize.width;
		patch.upper = rand() % imgSize.height;

		// Choose the size of the patch.
		patch.width = (int)((1 - sqrtf(1 - (float)rand() / RAND_MAX)) * imgSize.width);
		patch.height = (int)((1 - sqrtf(1 - (float)rand() / RAND_MAX)) * imgSize.height);

		// Check if the feature is valid.
		// Is this patch inside?
		if (!imgSize.IsIn(patch))
			continue;
		// Is this patch big enough?
		if (patch.height * patch.width < minimumArea)
			continue;

		// We have found a proper subregion.
		valid = true;
	}

	initSize = imgSize;
	curSize = initSize;
	scaledWidth = scaledHeight = 1.0f;
	scaledPatch = patch;
}

void RGIFeature::GetInitialDistribution(EstimatedGaussianDistribution<NUM_RGI_BINS> *distribution) const {
	distribution->SetValues(initMean, initSigma);
}

bool RGIFeature::Extract(const IntegralImage *intImage, const Rect &roi, Feature *feature) {
	feature->Resize(NUM_RGI_BINS);

	Point2D offset;
	offset = roi;

	if (curSize.width != roi.width || curSize.height != roi.height) {
		curSize = roi;
		if (!(initSize == curSize)) {
			scaledWidth = (float)curSize.width / initSize.width;
			scaledHeight = (float)curSize.height / initSize.height;

			scaledPatch.width = (int)floorf((float)patch.width * scaledWidth + 0.5f);
			scaledPatch.height = (int)floorf((float)patch.height * scaledHeight + 0.5f);

			if (scaledPatch.height < minimumSize.height || scaledPatch.width < minimumSize.width) {
				// After rescaling, the patch is too small, the feature is invalid.
				scaledWidth = 0.0f;
				return false;
			}

			scaledPatch.left = (int)floorf((float)patch.left * scaledWidth + 0.5f);
			scaledPatch.upper = (int)floorf((float)patch.upper * scaledHeight + 0.5f);
		}
		else {
			// The new patch is the same size as the original one.
			scaledWidth = scaledHeight = 1.0f;
			scaledPatch = patch;
		}
	}

	// Finish rescaling.
	if (scaledWidth == 0.0f)
		return false;

	// Get the RGI histogram.
	intImage->GetSum(scaledPatch + offset, feature->data);

	// Normalize the histogram.
	NormalizeHistogram(feature->data, NUM_RGI_BINS);

	return true;
}