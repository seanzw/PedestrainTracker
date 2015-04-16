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
	virtual bool Update(feat *feature, int target);

	// Evaluate this feature.
	virtual bool Eval(feat *feature) const;

	// Get the score.
	virtual feat GetValue(feat *feature) const;
};

#endif