/**********************************************
 ClassifierSelector class.
 Select the best classifier from some weak classifiers.
 Used in online boosting classifier.
 Author: Zhengrong Wang.
 **********************************************/

#ifndef CLASSIFIER_SELECTOR_CLASS
#define CLASSIFIER_SELECTOR_CLASS

#include "WeakClassifierHoG.h"

class ClassifierSelector {
public:
	// Constructor.
	ClassifierSelector(int numWeakClassifier, int numBackup = 2);
	ClassifierSelector(int numWeakClassifier, WeakClassifier **weaks, int numBackup = 2);

	virtual ~ClassifierSelector();

	// Train the selector.
	// @param feature: the feature vector;
	// @param target: 1 for pos, others for neg;
	// @param importance: the weight of this sample.
	void Train(feat *feature, bool target, float importance);

	// Return the error rate of this seletor, or any specific classifier.
	float GetErrors(int index = -1) const;

	// Select the best classifier.
	// @param importance: the weight of this sample.
	// @param errors: a buffer contains the error rates of each classifier.
	// @param errorMask: true if the classifer makes mistake on this sample.
	// @return: new selected classifier.
	virtual int SelectBestClassifer(float importance, float *errors, bool *errorMask);

	// Replace the weakest classifier.
	// @param errors: a buffer contains the error rate.
	// @return: the index of the replaced classifier.
	virtual int ReplaceWeakestClassifier(float *errors);

	// Evaluate a feature.
	// @return: 1 for pos, 0 for neg.
	bool Evaluate(feat *feature) const;

	// Get the score.
	float GetValue(feat *feature, int index = -1) const;

	// Get or set the classifier pool.
	WeakClassifier **GetClassifierPool() const;
	void SetClassifierPool(WeakClassifier **weaks);

	// 
private:
	// Data.
	// Classifier pool.
	WeakClassifier **m_weakClassifiers;

	int m_numWeakClassifer;				// Number of classifiers.
	int m_numBackup;					// Number of backup classifiers.
	int m_selectedClassifier;			// Index of the selected classifier.
	int m_nextBackup;					// Index of the next backup classifier.

	// Learning rate for each weak classifier.
	// Used to estimate the error rate.
	float *m_wCorrect;
	float *m_wWrong;

	// Are we using weak classifiers outside?
	// Used in destructor.
	bool m_isReferenced;

	// Generate the weak classifier.
};

#endif