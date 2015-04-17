#include "ClassifierSelector.h"

ClassifierSelector::ClassifierSelector(int numWeakClassifiers, int numBackup) {

	// Set the data.
	m_numWeakClassifer = numWeakClassifiers;
	m_numBackup = numBackup;
	m_weakClassifiers = new WeakClassifier*[m_numWeakClassifer + m_numBackup];
	m_selectedClassifier = 0;
	m_nextBackup = m_numWeakClassifer;
	m_wCorrect = new float[m_numWeakClassifer + m_numBackup];
	m_wWrong = new float[m_numWeakClassifer + m_numBackup];
	memset((void *)m_wCorrect, 0, sizeof(float) * (m_numWeakClassifer + m_numBackup));
	memset((void *)m_wWrong, 0, sizeof(float) * (m_numWeakClassifer + m_numBackup));

	// Initialize the weak classifiers.
	for (int i = 0; i < m_numWeakClassifer + m_numBackup; i++) {
		m_weakClassifiers[i] = new WeakClassifierHoG();
	}

	// We newed the weak classifiers.
	m_isReferenced = false;

	// Initialize the data.
	// This makes the error rate 0.5.
	for (int i = 0; i < m_numWeakClassifer + m_numBackup; i++) {
		m_wWrong[i] = m_wCorrect[i] = 1;
	}

}

ClassifierSelector::ClassifierSelector(int numWeakClassifiers, 
	WeakClassifier **weaks, int numBackup) {
	// Set the data.
	m_numWeakClassifer = numWeakClassifiers;
	m_numBackup = numBackup;
	m_weakClassifiers = weaks;
	m_selectedClassifier = 0;
	m_nextBackup = m_numWeakClassifer;
	m_wCorrect = new float[m_numWeakClassifer + m_numBackup];
	m_wWrong = new float[m_numWeakClassifer + m_numBackup];
	memset((void *)m_wCorrect, 0, sizeof(float) * (m_numWeakClassifer + m_numBackup));
	memset((void *)m_wWrong, 0, sizeof(float) * (m_numWeakClassifer + m_numBackup));

	// We use weak classifiers from outside.
	m_isReferenced = true;

	// Initialize the data.
	// This makes the error rate 0.5.
	for (int i = 0; i < m_numWeakClassifer + m_numBackup; i++) {
		m_wWrong[i] = m_wCorrect[i] = 1;
	}
}

ClassifierSelector::~ClassifierSelector() {
	if (!m_isReferenced) {
		for (int i = 0; i < m_numWeakClassifer + m_numBackup; i++) {
			delete m_weakClassifiers[i];
		}
		delete[] m_weakClassifiers;
	}
	delete[] m_wCorrect;
	delete[] m_wWrong;
}

bool ClassifierSelector::Evaluate(feat *feature) const {
	return m_weakClassifiers[m_selectedClassifier]->Evaluate(feature);
}

float ClassifierSelector::GetValue(feat *feature, int index) const {
	if (index < 0 || index >= m_numWeakClassifer) {
		return m_weakClassifiers[m_selectedClassifier]->GetValue(feature);
	}
	else {
		return m_weakClassifiers[index]->GetValue(feature);
	}
}

void ClassifierSelector::Train(feat *feature, bool target, float importance) {

	// Get the poisson value.
	double A = 1;
	int i;
	for (i = 0; i < 11; i++) {
		double U_K = (double)rand() / RAND_MAX;
		A *= U_K;
		if (A < exp(-importance))
			break;
	}

	// Train all the classifiers for i times.
	for (int curWeak = 0; curWeak < m_numWeakClassifer + m_numBackup; curWeak++) {
		for (int j = 0; j < i; j++) {
			m_weakClassifiers[curWeak]->Update(feature, target);
		}
	}
}

float ClassifierSelector::GetErrors(int index) const {

	if (index < 0 || index >= m_numWeakClassifer)
		index = m_selectedClassifier;

	return m_wWrong[index] / (m_wCorrect[index] + m_wWrong[index]);
}

int ClassifierSelector::SelectBestClassifer(float importance, float *errors, bool *errorMask) {
	int newSelected = m_selectedClassifier;
	int minError = FLT_MAX;

	for (int i = 0; i < m_numWeakClassifer; i++) {
		if (errorMask[i]) {
			m_wWrong[i] += importance;
		}
		else {
			m_wCorrect[i] += importance;
		}
		
		errors[i] = m_wWrong[i] / (m_wCorrect[i] + m_wWrong[i]);

		if (errors[i] < minError) {
			minError = errors[i];
			newSelected = i;
		}
	}

	m_selectedClassifier = newSelected;
	return m_selectedClassifier;
}

int ClassifierSelector::ReplaceWeakestClassifier(float *errors) {
	float maxError = 0.0f;
	int index = -1;

	// Find the classifier with the largest error.
	for (int i = 0; i < m_numWeakClassifer; i++) {
		if (errors[i] > maxError) {
			maxError = errors[i];
			index = i;
		}
	}

	// Some tests.
	assert(index > -1);
	assert(index != m_selectedClassifier);

	// Replace.
	if (maxError > errors[m_nextBackup]) {
		delete m_weakClassifiers[index];
		m_weakClassifiers[index] = m_weakClassifiers[m_nextBackup];
		m_wWrong[index] = m_wWrong[m_nextBackup];
		m_wWrong[m_nextBackup] = 1;
		m_wCorrect[index] = m_wCorrect[m_nextBackup];
		m_wCorrect[m_nextBackup] = 1;

		m_weakClassifiers[m_nextBackup] = new WeakClassifierHoG();

		// Find the next backup.
		m_nextBackup++;
		if (m_nextBackup == m_numBackup + m_numWeakClassifer)
			m_nextBackup = m_numWeakClassifer;

		return index;
	}
	else {
		return -1;
	}


}