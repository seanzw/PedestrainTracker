/**
 * Using Haar Feature to construct a weak classifier.
 * Also estimate the positive and negative cluster with Kalman filter.
 * @author Zhengrong Wang.
 */

#ifndef WEAKCLASSIFIER_HAAR_HEADER
#define WEAKCLASSIFIER_HAAR_HEADER

#include "WeakClassifier.h"
#include "ClassifierThreshold.h"
#include "HaarFeature.h"

class WeakClassifierHaar : public WeakClassifier {
public:
	WeakClassifierHaar(const Size &patchSize);
	virtual ~WeakClassifierHaar();

	bool Update(const IntegralImage *intImage, const Rect &roi, int target);
	int Classify(const IntegralImage *intImage, const Rect &roi, float scale = 1.0f);
	float Evaluate(const IntegralImage *intImage, const Rect &roi);

	virtual void Initialize(const Size &patchSize);

	void ResetPosDist();

private:
	ClassifierThreshold<1> *thresholder;
	HaarFeature *haarFeature;

	Feature feature;
};

#endif