/************************************************************
 WeakClassifierHaar class.
 Using Haar Feature to construct a weak classifier.
 Also estimate the positive and negative cluster with Kalman filter.
 Author: Zhengrong Wang.
 ************************************************************/

#ifndef WEAKCLASSIFIER_HAAR_HEADER
#define WEAKCLASSIFIER_HAAR_HEADER

#include "WeakClassifier.h"
#include "ClassifierThreshold.h"
#include "HaarFeature.h"

class WeakClassifierHaar : public WeakClassifier {
public:
	WeakClassifierHaar(Size patchSize);
	virtual ~WeakClassifierHaar();

	bool Update(IntegralImage *intImage, const Rect &roi, int target);
	bool Classify(const IntegralImage *intImage, const Rect &roi, float scale = 1.0f);


	// Reset the distribution.
	void ResetPosDist();
	void InitPosDist();


private:
	ClassifierThreshold<1> thresholder;
	HaarFeature *haarFeature;

	void GenerateRandomClassifier();
};

#endif