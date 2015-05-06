/**
 * Select the best classifier from some weak classifiers.
 * Here we only use WeakClassifierHaar.
 * Used in online boosting classifier.
 * @author Zhengrong Wang.
 */

#ifndef CLASSIFIER_SELECTOR_CLASS
#define CLASSIFIER_SELECTOR_CLASS

#include "WeakClassifierHaar.h"
#include "WeakClassifierRGI.h"

class ClassifierSelector {
public:
	/**
	 * Build a classifier selector with some WeakClassifierHaars.
	 *
	 * @param numW		# weak classifiers
	 * @param patchSize	the patch size used in weak classifiers
	 * @param numB		# weak classifiers for subsititution
	 */
	ClassifierSelector(int numW, const Size &patchSize, int numB = 2);

	/**
	 *Build a classifier selector from some outside weakclassifier array.
	 * @param weaks: the array of weak classifiers.
	 */
	ClassifierSelector(int numW, WeakClassifier **weaks, int numB = 2);

	virtual ~ClassifierSelector();

	/**
	 * Train the weak classifiers.
	 *
	 * @param intImage		the integral image
	 * @param roi			the region of the target
	 * @param target		1 for pos, -1 for neg
	 * @param importance	the weight of this sample
	 * @param out errMask	update the error mask array
	 */
	void Train(const IntegralImage *intImage, const Rect &roi, int target, float importance, bool *errMask);

	/**
	 * @return the error rate of this seletor, or any specific classifier.
	 */
	float GetError(int index = -1) const;

	/** 
	 * Select the best classifier.
	 *
	 * @param in  importance	the weight of this sample
	 * @param in  errorMask		true if the classifer makes mistake on this sample
	 * @param out errors		a buffer contains the error rates of each classifier
	 * @return					index of new selected classifier
	 */
	virtual int SelectBestClassifer(float importance, const bool *errorMask, float *errors);

	/** 
	 * Replace the weakest classifier.
	 *
	 * @param sumErrors	a buffer contains the sum of error rate. The biggest weak classifier will be replaced
	 * @return			the index of the replaced classifier
	 */
	virtual int ReplaceWeakestClassifier(float *sumErrors, const Size &patchSize);

	/**
	 * Only replace the weight.
	 * Used when the weak classifier is from outside.
	 */
	virtual void ReplaceWeakestClassifierStatistic(int src, int dst);

	/**
	 * Evaluate a feature.
	 *
	 * @return	1 for pos, -1 for neg
	 */
	int Classify(const IntegralImage *intImage, const Rect &roi);

	float Evaluate(const IntegralImage *intImage, const Rect &roi, int indexWeak = -1);

	WeakClassifier **GetClassifierPool() const { return weakClassifiers; }
	void SetClassifierPool(WeakClassifier **weaks);

	int GetNewBackup() { return nextBackup; }

private:

	WeakClassifier **weakClassifiers;

	int numWeakClassifer;			// Number of classifiers.
	int numBackup;					// Number of backup classifiers.
	int selectedClassifier;			// Index of the selected classifier.
	int nextBackup;					// Index of the next backup classifier.

	/**
	 * Learning rate for each weak classifier.
	 * Used to estimate the error rate.
	 */
	float *wCorrect;
	float *wWrong;

	/**
	 * Are we using weak classifiers outside?
	 * Used in destructor.
	 */
	bool isReferenced;
};

#endif