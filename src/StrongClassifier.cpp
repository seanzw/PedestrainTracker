#include "StrongClassifier.h"

StrongClassifier::StrongClassifier(int numSelector, 
	int numWeakClassifier, int numBackup) {

	// Set up the data.
	m_numSelector = numSelector;
	m_totalWeakClassifiers = numWeakClassifier + numBackup;

	m_alpha = new float[m_numSelector];
	memset((void *)m_alpha, 0, sizeof(float) * m_numSelector);

}

StrongClassifier::~StrongClassifier() {
	delete[] m_alpha;
}

float StrongClassifier::Evaluate(feat *feature) const {
	float value = 0.0f;

	for (int i = 0; i < m_numSelector; i++) {
		value += m_selectors[i]->GetValue(feature) * m_alpha[i];
	}

	return value;
}

bool StrongClassifier::Update(feat *feature, bool target,
	float importance) {
	return true;
}

