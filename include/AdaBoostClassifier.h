/***********************************************
 AdaBoostClassifier class.
 Use many weak classifier to build a strong classifier.
 Author: Zhengrong Wang.
 ***********************************************/

#ifndef ADABOOST_CLASSIFIER_HEADER
#define ADABOOST_CLASSIFIER_HEADER

#include "WeakClassifierHoG.h"

class AdaBoostClassifier : public Classifier {
public:
	// Build the AdaBoost Classifier from files.
	AdaBoostClassifier(const char *filepath);

	~AdaBoostClassifier();

	int Classify(const IntegralImage *intImage, const Rect &roi, float scale);

private:
	// Data.
	// All the weak classifiers.
	WeakClassifierHoG **weaks;

	// The number of stages.
	int numStages;

	// The number of weak classifiers for each stage.
	int *numWeaks;

	// The threshold for each stage.
	float *threshold;
};

#endif