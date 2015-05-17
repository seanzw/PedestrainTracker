#include "WeakClassifierRGI.h"


WeakClassifierRGI::WeakClassifierRGI(const Size &patchSize) {
	rgiFeature = new RGIFeature(patchSize);
	GenerateRandomClassifier();

	// Set the initial distribution.
	rgiFeature->GetInitialDistribution((EstimatedGaussianDistribution<NUM_RGI_BINS> *)thresholder->GetDistribution(1));
	rgiFeature->GetInitialDistribution((EstimatedGaussianDistribution<NUM_RGI_BINS> *)thresholder->GetDistribution(-1));
}

WeakClassifierRGI::~WeakClassifierRGI() {
	delete rgiFeature;
	delete thresholder;
}

void WeakClassifierRGI::ResetPosDist() {
	rgiFeature->GetInitialDistribution((EstimatedGaussianDistribution<NUM_RGI_BINS> *)thresholder->GetDistribution(1));
	rgiFeature->GetInitialDistribution((EstimatedGaussianDistribution<NUM_RGI_BINS> *)thresholder->GetDistribution(-1));
}

void WeakClassifierRGI::Initialize(const Size &patchSize) {
	// Reset the RGI feature extractor.
	rgiFeature->Reset(patchSize);

	// Reset the classifier threshold.
	thresholder->Reset();

	// Set the initial distribution.
	rgiFeature->GetInitialDistribution((EstimatedGaussianDistribution<NUM_RGI_BINS> *)thresholder->GetDistribution(1));
	rgiFeature->GetInitialDistribution((EstimatedGaussianDistribution<NUM_RGI_BINS> *)thresholder->GetDistribution(-1));
}

void WeakClassifierRGI::GenerateRandomClassifier() {
	thresholder = new ClassifierThreshold<NUM_RGI_BINS>();
}

bool WeakClassifierRGI::Update(const IntegralImage *intImage, const Rect &roi, int target) {
	bool valid = rgiFeature->Extract(intImage, roi, &feature);

	if (!valid) {
		return true;
	}

	thresholder->Update(feature, target);
	return thresholder->Classify(feature) != target;
}

int WeakClassifierRGI::Classify(const IntegralImage *intImage, const Rect &roi, float scale) {
	bool valid = rgiFeature->Extract(intImage, roi, &feature);

	if (!valid) {
		return -1;
	}

	return thresholder->Classify(feature);
}

float WeakClassifierRGI::Evaluate(const IntegralImage *intImage, const Rect &roi) {
	bool valid = rgiFeature->Extract(intImage, roi, &feature);
	if (!valid) {
		return 0.0f;
	}
	return thresholder->Classify(feature) == 1 ? 1.0f : 0.0f;
}
