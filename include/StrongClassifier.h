/********************************************************
 StrongClassifier class.
 Use ClassSelector to build a strong classifier.
 Author: Zhengrong Wang.

 Notice: The feature extractor should be prepared
 (if there are any preprocess procedure), the caller
 of strong classifier should do that.

 ********************************************************/

#ifndef STRONG_CLASSIFIER_HEADER
#define STRONG_CLASSIFIER_HEADER

#include "ClassifierSelector.h"

class StrongClassifier {
public:
	StrongClassifier(int numSelector, int numWeakClassifier, int numBackup = 0);
	virtual ~StrongClassifier();

	// Evaluate a region.
	virtual float Evaluate(feat *feature) const;

	// Update the strong classifier.
	virtual bool Update(feat *feature, 
		bool target, float importance = 1.0f);

	// 

protected:
	int m_numSelector;
	int m_totalWeakClassifiers;

	ClassifierSelector **m_selectors;

	// The weight of each selector.
	float *m_alpha;

};


#endif