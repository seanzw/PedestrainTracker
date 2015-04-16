/**********************************************
 ClassifierSelector class.
 Used in online boosting classifier.
 Author: Zhengrong Wang.
 **********************************************/

#ifndef CLASSIFIER_SELECTOR_CLASS
#define CLASSIFIER_SELECTOR_CLASS

#include "WeakClassifierHoG.h"

class ClassifierSelector {
public:
	// Constructor.
	ClassifierSelector(int numWeakClassifier, int iterationInit);
	ClassifierSelector(int numWeakClassifier, int iterationInit, WeakClassifier **weaks);

	virtual ~ClassifierSelector();

	// Train the selector.
	// @param feature: the feature vector;
	// @param target: 1 for pos, others for neg;
	// @param importance: the weight of this sample.
	void Train(feat *feature, int target, float importance);

	// Return the error rate of this seletor, or any specific classifier.
	float GetErrors(int index = -1) const;

	// Select the best classifier.
	virtual int SelectBestClassifer(float importance);

	// Replace the weakest classifier.
	virtual int ReplaceWeakestClassifier();

	// Evaluate a feature.
	// @return: 1 for pos, 0 for neg.
	int Evaluate(feat *feature) const;

	// Get the score.
	float GetValue(feat *feature, int index = -1) const;

	// Get or set the classifier pool.
	WeakClassifier **GetClassifierPool() const;
	void SetClassifierPool(WeakClassifier **weaks);

	// 
private:
	// Data.
	// Classifier pool.
	WeakClassifier **weakClassifiers;

	int m_numWeakClassifer;				// Number of classifiers.
	int m_selectedClassifier;			// Index of the selected classifier.
	//float *m_wCorrect;
	//float *m_wWrong;

	int m_iterationInit;

	//
};

#endif