/**
 This is an interface.
 * @author Zhengrong Wang.
 */

#ifndef WEAKCLASSIFIER_HEADER
#define WEAKCLASSIFIER_HEADER

#include "Classifier.h"

class WeakClassifier : public Classifier {
public:
	WeakClassifier();
	virtual ~WeakClassifier();

	/**
	 * Update this weak classifer.
	 * Return true if after update, this weak classifer
	 * still can not classify this sample correctly.
	 */
	virtual bool Update(const IntegralImage *intImage, const Rect &roi, int target);

	/**
	 Initialize the weak classifier.
	 */
	virtual void Initialize(const Size &patchSize);

	/**
	 * @return	[0.0, 1.0]
	 */
	virtual float Evaluate(const IntegralImage *intImage, const Rect &roi);
};

#endif