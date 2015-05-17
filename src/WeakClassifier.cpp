#include "WeakClassifier.h"

WeakClassifier::WeakClassifier() {}

WeakClassifier::~WeakClassifier() {}

bool WeakClassifier::Update(const IntegralImage *intImage, const Rect &roi, int target) {
	return true;
}

// Do nothing here.
void WeakClassifier::Initialize(const Size &patchSize) {

}

float WeakClassifier::Evaluate(const IntegralImage *intImage, const Rect &roi) {
	return 0.0f;
}