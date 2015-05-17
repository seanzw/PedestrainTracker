#include "StrongClassifier.h"

StrongClassifier::StrongClassifier(int numSelector, int numWeakClassifier, 
	const Size &patchSize, bool useFeatureReplace, int numBackup) {

	// Set up the data.
	this->numSelector = numSelector;
	this->useFeatureReplace = useFeatureReplace;
	this->totalWeakClassifiers = numWeakClassifier + numBackup;
	this->patchSize = patchSize;

	this->alpha = new float[this->numSelector];
	memset((void *)this->alpha, 0, sizeof(float) * this->numSelector);
}

StrongClassifier::~StrongClassifier() {
	delete[] this->alpha;
}

float StrongClassifier::Evaluate(const IntegralImage *intImage, const Rect &roi) const {
	float value = 0.0f;

	for (int i = 0; i < this->numSelector; i++) {
		value += this->selectors[i]->Evaluate(intImage, roi) * this->alpha[i];
	}

	return value;
}

void StrongClassifier::Initialize(const Size &patchSize) {
	this->patchSize = patchSize;

	// Reset the alpha.
	memset((void *)this->alpha, 0, sizeof(float) * this->numSelector);
}

bool StrongClassifier::Update(const IntegralImage *intImage, const Rect &roi, int target,
	float importance) {
	return false;
}

