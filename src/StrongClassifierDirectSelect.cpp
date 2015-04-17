#include "StrongClassifierDirectSelect.h"

StrongClassifierDirectSelect::StrongClassifierDirectSelect(int numSelectors,
	int numWeakClassifiers, int numBackups)
	: StrongClassifier(numSelectors, numWeakClassifiers, numBackups) {

	m_selectors = new ClassifierSelector*[m_numSelector];

	// Initialize the first selector.
	m_selectors[0] = new ClassifierSelector(numWeakClassifiers, numBackups);

	for (int i = 1; i < m_numSelector; i++) {
		m_selectors[i] = new ClassifierSelector(numWeakClassifiers, m_selectors[0]->GetClassifierPool(), numBackups);
	}

	m_errMask = new bool[m_totalWeakClassifiers];
	m_errors = new float[m_totalWeakClassifiers];
	m_sumErrors = new float[m_totalWeakClassifiers];

}

StrongClassifierDirectSelect::~StrongClassifierDirectSelect() {
	delete[] m_errMask;
	delete[] m_errors;
	delete[] m_sumErrors;

	for (int i = 0; i < m_numSelector; i++) {
		delete m_selectors[i];
	}

	delete[] m_selectors;
}

bool StrongClassifierDirectSelect::Update(feat *feature, 
	bool target,
	float importance) {

	// Initialize the buffer.
	memset((void *)m_errMask, 0, sizeof(bool) * m_totalWeakClassifiers);
	memset((void *)m_errors, 0, sizeof(float) * m_totalWeakClassifiers);
	memset((void*)m_sumErrors, 0, sizeof(float) * m_totalWeakClassifiers);

	// Train all the weak classifiers.
	// And get the errMask
	m_selectors[0]->Train(feature, target, importance, m_errMask);

	// Select the best weak classifier for each selector.
	for (int i = 0; i < m_numSelector; i++) {

		// Select the best weak classifiers and get the error rate.
		int selected = m_selectors[i]->SelectBestClassifer(importance, m_errors, m_errMask);

		if (m_errors[selected] > 0.5) {
			// If the error rate is greater than 0.5,
			// we won't use it.
			m_alpha[i] = 0.0f;
		}
		else {
			// Calculate the alpha.
			m_alpha[i] = logf((1.0f - m_errors[selected]) / m_errors[selected]);
		}

		// Update the importance;
		if (m_errMask[selected]) {
			importance *= sqrtf((1.0f - m_errors[selected]) / m_errors[selected]);
		}
		else {
			importance *= sqrtf(m_errors[selected] / (1.0f - m_errors[selected]));
		}

		// Sum up the errors.
		for (int j = 0; j < m_totalWeakClassifiers; i++) {
			if (m_errors[j] != FLT_MAX && m_sumErrors[i] > 0.0f) {
				m_sumErrors[j] += m_errors[j];
			}
		}


		// Mark this weak classifier as used.
		m_sumErrors[selected] = -1;
		m_errors[selected] = FLT_MAX;

	}

	// If we are gonna replace the weakest classifier...
	if (m_useFeatureReplace) {
		int replaced = m_selectors[0]->ReplaceWeakestClassifier(m_sumErrors);

		if (replaced > 0) {
			// We have successfully replaced a classifier.
			// We have to clear the weight in other selectors.
			for (int i = 1; i < m_numSelector; i++) {
				m_selectors[i]->ReplaceWeakestClassifierStatistic(m_selectors[0]->GetNewBackup(), replaced);
			}
		}
	}

	return true;
}
