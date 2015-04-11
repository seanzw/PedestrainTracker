#include "Classifier.h"


AdaBoostClassifier::AdaBoostClassifier(const char *filepath) {
	printf("Read in classifiers' information...\n");
	char filename[256];

	// Read the global description file.
	sprintf_s(filename, "%s%d.txt", filepath, 0);
	std::ifstream in;
	in.open(filename);
	if (!in.is_open()) {
		printf("Can't open file %s, exit!\n", filename);
	}
	in >> numStages;
	numWeaks = new int[numStages];

	// Number of all the weak classifiers.
	int sum = 0;
	for (int i = 0; i < numStages; i++) {
		in >> numWeaks[i];
		sum += numWeaks[i];
	}
	in.close();

	// Allocate the data.
	data = new feat[sum * SIZE_WEAK];

	// The current weak classifier.
	feat *cur = data;

	for (int i = 0; i < numStages; i++)
	{
		sprintf_s(filename, "%s%d.txt", filepath, i + 1);

		in.open(filename);
		if (!in.is_open()) {
			printf("Can't open file %s, exit!\n", filename);
		}

		int test;
		in >> test;

		// Read in the weak classifiers.
		for (int j = 0; j < numWeaks[i]; j++) {
			in >> cur[H_OFFSET];
			in >> cur[W_OFFSET];
			in >> cur[HEIGHT_OFFSET];
			in >> cur[WIDTH_OFFSET];
			in >> cur[Z_OFFSET];
			in >> cur[MIN_OFFSET];
			in >> cur[MAX_OFFSET];
			for (int k = 0; k < NUM_PROJ; k++) {
				in >> cur[PROJ_OFFSET + k];
			}
			for (int k = 0; k < NUM_HISTO; k++) {
				in >> cur[HISTO_OFFSET + k];
			}

			// Read in the threshold for the last weak classifier.
			if (j == numWeaks[i] - 1) {
				in >> cur[THRE_OFFSET];
			}

			// Move the cur pointer.
			cur += SIZE_WEAK;
		}

		in.close();
	}
	printf("Reading process completed!\n");
}

AdaBoostClassifier::~AdaBoostClassifier() {
	delete[] numWeaks;
	delete[] data;
}

bool AdaBoostClassifier::Classify(int x, int y, feat scale, FeatureExtractor &featureExt) const {

	// The pointer to the current weak classifier.
	feat *cur = data;
	HoGFeature feature;
	for (int stage = 0; stage < numStages; stage++) {
		feat threshold = 0.0;
		for (int weak = 0; weak < numWeaks[stage]; weak++) {
			int xx = x + (int)(cur[H_OFFSET] * scale) - 1;
			int yy = y + (int)(cur[W_OFFSET] * scale) - 1;
			int scaleWW = (int)(cur[WIDTH_OFFSET] * scale);
			int scaleHH = (int)(cur[HEIGHT_OFFSET] * scale);

			featureExt.Extract(scaleWW, scaleHH, xx, yy, &feature);

			feat sum = 0.0;
			for (int k = 0; k < 36; k++)
				sum += feature.hogs[k];

			// Normalize the feature.
			if (sum > 100.0 * scale) {
				feat invSum = 1.0 / sum;
				for (int k = 0; k < 36; k++)
					feature.hogs[k] *= invSum;
			} else {
				return false;
			}

			// Projection.
			feat projValue = 0.0;
			for (int k = 0; k < NUM_PROJ; k++) {
				projValue += cur[PROJ_OFFSET + k] * feature.hogs[k];
			}

			// Judge the district.
			int district = District(projValue, cur[MIN_OFFSET], cur[MAX_OFFSET]);
			threshold += cur[HISTO_OFFSET + district];

			// What the fxxk is going on here??
			if (weak == numWeaks[stage] - 1 && threshold < cur[THRE_OFFSET]) {
				return false;
			}

			// Move the current weak classifier.
			cur += SIZE_WEAK;
		}
	}
	return true;
}