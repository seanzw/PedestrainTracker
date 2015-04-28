/**
 * AdaBoostClassifier class.
 * Use many weak classifier to build a strong classifier.
 * @author Zhengrong Wang.
 */

#ifndef ADABOOST_CLASSIFIER_HEADER
#define ADABOOST_CLASSIFIER_HEADER

#include "WeakClassifierHoG.h"

class AdaBoostClassifier : public Classifier {
public:
	AdaBoostClassifier(const char *filepath);

	~AdaBoostClassifier();

	int Classify(const IntegralImage *intImage, const Rect &roi, float scale);

private:
	WeakClassifierHoG **weaks;

	int numStages;

	int *numWeaks;

	float *threshold;
};

#endif