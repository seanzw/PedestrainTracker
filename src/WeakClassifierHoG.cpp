#include "WeakClassifierHoG.h"

// TO DO:
// Initialize the classifier randomly
WeakClassifierHoG::WeakClassifierHoG() {
	
}

WeakClassifierHoG::~WeakClassifierHoG() {

}

void WeakClassifierHoG::ResetPosDist() {
	// TO DO:
}

bool WeakClassifierHoG::Evaluate(feat *feature) const {
	return classifier.Eval(feature);
}

feat WeakClassifierHoG::GetValue(feat *feature) const {
	return classifier.GetValue(feature);
}

bool WeakClassifierHoG::Update(feat *feature, bool target) {
	classifier.Update(feature, target);
	return classifier.Eval(feature) != target;
}