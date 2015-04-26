/**********************************************
 StrongClassifierDirectSelect class.
 This strong classifier directly select the best
 weak classifier and replace the worst one.
 **********************************************/

#ifndef STRONG_CLASSIFIER_DIRECT_HEADER
#define STRONG_CLASSIFIER_DIRECT_HEADER

#include "StrongClassifier.h"

class StrongClassifierDirectSelect : public StrongClassifier {
public:
	StrongClassifierDirectSelect(int numSelectors,
		int numWeakClassifiers, const Size &patchSize, int numBackups = 0);

	virtual ~StrongClassifierDirectSelect();

	bool Update(const IntegralImage *intImage, const Rect &roi, int target, float importance = 1.0f);

private:
	// Some buffer used in selectors.
	bool *errMask;
	float *errors;
	float *sumErrors;

};

#endif