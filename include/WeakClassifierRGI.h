/**
 * Using RGI feature to construct a weak classifier.
 * @author Zhengrong Wang
 */

#ifndef WEAKCLASSIFIER_RGI_HEADER
#define WEAKCLASSIFIER_RGI_HEADER

#include "WeakClassifier.h"
#include "ClassifierThreshold.h"
#include "RGIFeature.h"

class WeakClassifierRGI : public WeakClassifier {
public:
	WeakClassifierRGI(const Size &patchSize);
	virtual ~WeakClassifierRGI();

	bool Update(const IntegralImage *intImage, const Rect &roi, int target);
	int Classify(const IntegralImage *intImage, const Rect &roi, float scale = 1.0f);
	float Evaluate(const IntegralImage *intImage, const Rect &roi);

	void ResetPosDist();

private:
	ClassifierThreshold<NUM_RGI_BINS> *thresholder;
	RGIFeature *rgiFeature;

	Feature feature;

	void GenerateRandomClassifier();
};

#endif
