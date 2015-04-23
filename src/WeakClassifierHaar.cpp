#include "WeakClassifierHaar.h"

WeakClassifierHaar::WeakClassifierHaar(const Size &patchSize) {
	haarFeature = new HaarFeature(patchSize);
	GenerateRandomClassifier();

	// Set the initial distribution.
	haarFeature->GetInitialDistribution((EstimatedGaussianDistribution<1> *)thresholder->GetDistribution(1));
	haarFeature->GetInitialDistribution((EstimatedGaussianDistribution<1> *)thresholder->GetDistribution(-1));
}

WeakClassifierHaar::~WeakClassifierHaar() {
	delete haarFeature;
	delete thresholder;
}

void WeakClassifierHaar::ResetPosDist() {
	haarFeature->GetInitialDistribution((EstimatedGaussianDistribution<1> *)thresholder->GetDistribution(1));
	haarFeature->GetInitialDistribution((EstimatedGaussianDistribution<1> *)thresholder->GetDistribution(-1));
}

void WeakClassifierHaar::GenerateRandomClassifier() {
	thresholder = new ClassifierThreshold<1>();
}

bool WeakClassifierHaar::Update(const IntegralImage *intImage, const Rect &roi, int target) {
	bool valid = haarFeature->Extract(intImage, roi, &feature);

	if (!valid) {
		return true;
	}

	thresholder->Update(feature, target);
	return thresholder->Classify(feature) != target;
}

int WeakClassifierHaar::Classify(const IntegralImage *intImage, const Rect &roi, float scale) {
	bool valid = haarFeature->Extract(intImage, roi, &feature);

	if (!valid) {
		return -1;
	}

	return thresholder->Classify(feature);
}

