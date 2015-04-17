/****************************************
 A weak classifier using HoG
 Author: Zhengrong Wang.
 ****************************************/

#ifndef WEAKCLASSIFIER_HOG_HEADER
#define WEAKCLASSIFIER_HOG_HEADER

#include "WeakClassifier.h"
#include "ClassifierThreshold.h"

class WeakClassifierHoG : public WeakClassifier {
public:
	WeakClassifierHoG();
	~WeakClassifierHoG();

	bool Update(feat *feature, bool target);
	bool Evaluate(feat *feature) const;
	feat GetValue(feat *feature) const;

	// Reset distribution.
	void ResetPosDist();

private:
	// A 36 dimension classifier threshold.
	ClassifierThreshold<36> classifier;
};

#endif