/***********************************************
 WeakClassifier class.
 This is an interface.
 Author: Zhengrong Wang.
 ***********************************************/

#ifndef WEAKCLASSIFIER_HEADER
#define WEAKCLASSIFIER_HEADER

#include "Classifier.h"

class WeakClassifier : public Classifier {
public:
	WeakClassifier();
	virtual ~WeakClassifier();

	// Update this weak classifer.
	// Return true if after update, this weak classifer
	// still can not classify this sample correctly.
	virtual bool Update(const IntegralImage *intImage, const Rect &roi, int target);

};

#endif