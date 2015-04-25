#include "StrongClassifierDirectSelect.h"

StrongClassifierDirectSelect::StrongClassifierDirectSelect(int numSelectors,
	int numWeakClassifiers, int numBackups)
	: StrongClassifier(numSelectors, numWeakClassifiers, numBackups) {

	this->selectors = new ClassifierSelector*[this->numSelector];

	// Initialize the first selector.
	this->selectors[0] = new ClassifierSelector(numWeakClassifiers, numBackups);

	for (int i = 1; i < this->numSelector; i++) {
		this->selectors[i] = new ClassifierSelector(numWeakClassifiers, this->selectors[0]->GetClassifierPool(), numBackups);
	}

	this->errMask = new bool[this->totalWeakClassifiers];
	this->errors = new float[this->totalWeakClassifiers];
	this->sumErrors = new float[this->totalWeakClassifiers];

}

StrongClassifierDirectSelect::~StrongClassifierDirectSelect() {
	delete[] this->errMask;
	delete[] this->errors;
	delete[] this->sumErrors;

	for (int i = 0; i < this->numSelector; i++) {
		delete this->selectors[i];
	}

	delete[] this->selectors;
}

bool StrongClassifierDirectSelect::Update(const IntegralImage *intImage,
	const Rect &roi,
	int target,
	float importance) {

	// Initialize the buffer.
	memset((void *)this->errMask, 0, sizeof(bool) * this->totalWeakClassifiers);
	memset((void *)this->errors, 0, sizeof(float) * this->totalWeakClassifiers);
	memset((void*)this->sumErrors, 0, sizeof(float) * this->totalWeakClassifiers);

	// Train all the weak classifiers.
	// And get the errMask
	this->selectors[0]->Train(intImage, roi, target, importance, this->errMask);

	// Select the best weak classifier for each selector.
	for (int i = 0; i < this->numSelector; i++) {

		// Select the best weak classifiers and get the error rate.
		int selected = this->selectors[i]->SelectBestClassifer(importance, this->errMask, this->errors);

		if (this->errors[selected] > 0.5) {
			// If the error rate is greater than 0.5,
			// we won't use it.
			this->alpha[i] = 0.0f;
		}
		else {
			// Calculate the alpha.
			this->alpha[i] = logf((1.0f - this->errors[selected]) / this->errors[selected]);
		}

		// Update the importance;
		if (this->errMask[selected]) {
			importance *= sqrtf((1.0f - this->errors[selected]) / this->errors[selected]);
		}
		else {
			importance *= sqrtf(this->errors[selected] / (1.0f - this->errors[selected]));
		}

		// Sum up the errors.
		for (int j = 0; j < this->totalWeakClassifiers; i++) {
			if (this->errors[j] != FLT_MAX && this->sumErrors[i] > 0.0f) {
				this->sumErrors[j] += this->errors[j];
			}
		}


		// Mark this weak classifier as used.
		this->sumErrors[selected] = -1;
		this->errors[selected] = FLT_MAX;

	}

	// If we are gonna replace the weakest classifier...
	if (this->useFeatureReplace) {
		int replaced = this->selectors[0]->ReplaceWeakestClassifier(this->sumErrors, this->patchSize);

		if (replaced > 0) {
			// We have successfully replaced a classifier.
			// We have to clear the weight in other selectors.
			for (int i = 1; i < this->numSelector; i++) {
				this->selectors[i]->ReplaceWeakestClassifierStatistic(this->selectors[0]->GetNewBackup(), replaced);
			}
		}
	}

	return true;
}
