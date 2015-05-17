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

	virtual bool Update(const IntegralImage *intImage, const Rect &roi, int target);
	virtual int Classify(const IntegralImage *intImage, const Rect &roi, float scale = 1.0f);
	virtual float Evaluate(const IntegralImage *intImage, const Rect &roi);

	virtual void Initialize(const Size &patchSize);

	void ResetPosDist();

private:
	ClassifierThreshold<NUM_RGI_BINS> *thresholder;
	RGIFeature *rgiFeature;

	Feature feature;

	void GenerateRandomClassifier();
};

#endif
