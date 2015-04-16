#include "WeakClassifier.h"

WeakClassifier::WeakClassifier() {}

WeakClassifier::~WeakClassifier() {}

bool WeakClassifier::Eval(feat *feature) const {
	return true;
}

feat WeakClassifier::GetValue(feat *feature) const {
	return 0.0f;
}

bool WeakClassifier::Update(feat *feature, int target) {
	return true;
}