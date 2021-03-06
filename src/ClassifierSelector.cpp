#include "ClassifierSelector.h"

ClassifierSelector::ClassifierSelector(int numW, const Size &patchSize, int numB)
	: numWeakClassifer(numW), numBackup(numB) {

	// Set the data.
	weakClassifiers = new WeakClassifier*[numWeakClassifer + numBackup];
	selectedClassifier = 0;
	nextBackup = numWeakClassifer;
	wCorrect = new float[numWeakClassifer + numBackup];
	wWrong = new float[numWeakClassifer + numBackup];

	// Initialize the weak classifiers.
	for (int i = 0; i < numWeakClassifer + numBackup; i++) {
#ifdef USE_RGI_FEATURE
		weakClassifiers[i] = new WeakClassifierRGI(patchSize);
#else
		weakClassifiers[i] = new WeakClassifierHaar(patchSize);
#endif
	}

	// We newed the weak classifiers.
	isReferenced = false;

	// Initialize the data.
	// This makes the error rate 0.5.
	for (int i = 0; i < numWeakClassifer + numBackup; i++) {
		wWrong[i] = wCorrect[i] = 1;
	}
}

ClassifierSelector::ClassifierSelector(int numW, WeakClassifier **weaks, int numB)
	: numWeakClassifer(numW), numBackup(numB) {
	// Set the data.
	weakClassifiers = weaks;
	selectedClassifier = 0;
	nextBackup = numWeakClassifer;
	wCorrect = new float[numWeakClassifer + numBackup];
	wWrong = new float[numWeakClassifer + numBackup];

	// We use weak classifiers from outside.
	isReferenced = true;

	// Initialize the data.
	// This makes the error rate 0.5.
	for (int i = 0; i < numWeakClassifer + numBackup; i++) {
		wWrong[i] = wCorrect[i] = 1;
	}
}

ClassifierSelector::~ClassifierSelector() {
	if (!isReferenced) {
		for (int i = 0; i < numWeakClassifer + numBackup; i++) {
			delete weakClassifiers[i];
		}
		delete[] weakClassifiers;
	}
	delete[] wCorrect;
	delete[] wWrong;
}

void ClassifierSelector::Initialize(const Size &patchSize) {
	if (!isReferenced) {
		// Reset all the weak classifiers.
		for (int i = 0; i < numWeakClassifer + numBackup; i++) {
			weakClassifiers[i]->Initialize(patchSize);
		}
	}

	// Reset this selector state.
	selectedClassifier = 0;
	nextBackup = numWeakClassifer;

	// Initialize the data.
	// This makes the error rate 0.5.
	for (int i = 0; i < numWeakClassifer + numBackup; i++) {
		wWrong[i] = wCorrect[i] = 1;
	}
}

int ClassifierSelector::Classify(const IntegralImage *intImage, const Rect &roi) {
	return weakClassifiers[selectedClassifier]->Classify(intImage, roi);
}

void ClassifierSelector::Train(const IntegralImage *intImage, 
	const Rect &roi, int target, float importance, bool *errMask) {

	// Get the poisson value.
	double A = 1;
	int i;
	double thre = exp(-importance);
	for (i = 0; i < 11; i++) {
		double U_K = (double)rand() / RAND_MAX;
		A *= U_K;
		if (A < thre)
			break;
	}

	// Train all the classifiers for i times.
	for (int curWeak = 0; curWeak < numWeakClassifer + numBackup; curWeak++) {
		for (int j = 0; j < i; j++) {
			weakClassifiers[curWeak]->Update(intImage, roi, target);
		}
		errMask[curWeak] = weakClassifiers[curWeak]->Classify(intImage, roi) != target;
	}
}

float ClassifierSelector::GetError(int index) const {

	if (index < 0 || index >= numWeakClassifer)
		index = selectedClassifier;

	return wWrong[index] / (wCorrect[index] + wWrong[index]);
}

int ClassifierSelector::SelectBestClassifer(float importance, const bool *errorMask, float *errors) {
	int newSelected = selectedClassifier;
	float minError = FLT_MAX;

	for (int i = 0; i < numWeakClassifer + numBackup; i++) {
		if (errorMask[i]) {
			wWrong[i] += importance;
		}
		else {
			wCorrect[i] += importance;
		}

		// If this classifier has been selected by other selector.
		if (errors[i] == FLT_MAX)
			continue;
		
		errors[i] = wWrong[i] / (wCorrect[i] + wWrong[i]);

		// Update the minError.
		if (i < numWeakClassifer && errors[i] < minError) {
			minError = errors[i];
			newSelected = i;
		}
	}

	selectedClassifier = newSelected;
	return selectedClassifier;
}

int ClassifierSelector::ReplaceWeakestClassifier(float *sumErrors, const Size &patchSize) {
	float maxError = 0.0f;
	int index = -1;

	// Find the classifier with the largest error.
	for (int i = 0; i < numWeakClassifer; i++) {
		if (sumErrors[i] > maxError) {
			maxError = sumErrors[i];
			index = i;
		}
	}

	// Some tests.
	assert(index > -1);
	assert(index != selectedClassifier);

	// Find the next backup.
	nextBackup++;
	if (nextBackup == numBackup + numWeakClassifer)
		nextBackup = numWeakClassifer;

	// Replace.
	if (maxError > sumErrors[nextBackup]) {
		delete weakClassifiers[index];
		weakClassifiers[index] = weakClassifiers[nextBackup];
		wWrong[index] = wWrong[nextBackup];
		wWrong[nextBackup] = 1;
		wCorrect[index] = wCorrect[nextBackup];
		wCorrect[nextBackup] = 1;

#ifdef USE_RGI_FEATURE
		weakClassifiers[nextBackup] = new WeakClassifierRGI(patchSize);
#else
		weakClassifiers[nextBackup] = new WeakClassifierHaar(patchSize);
#endif

		return index;
	}
	else {
		return -1;
	}
}

void ClassifierSelector::ReplaceWeakestClassifierStatistic(int src, int dst) {
	// Some tests.
	assert(dst >= 0);
	assert(dst != selectedClassifier);
	assert(dst < numWeakClassifer);

	wCorrect[dst] = wCorrect[src];
	wWrong[dst] = wCorrect[dst];

	wCorrect[src] = 1.0f;
	wWrong[src] = 1.0f;
}

float ClassifierSelector::Evaluate(const IntegralImage *intImage, const Rect &roi, int indexWeak) {
	if (indexWeak < 0 || indexWeak >= numWeakClassifer) {
		return weakClassifiers[selectedClassifier]->Evaluate(intImage, roi);
	}
	return weakClassifiers[indexWeak]->Evaluate(intImage, roi);
}