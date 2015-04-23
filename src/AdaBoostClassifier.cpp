#include "AdaBoostClassifier.h"

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
	threshold = new float[numStages];

	// Number of all the weak classifiers.
	int sum = 0;
	for (int i = 0; i < numStages; i++) {
		in >> numWeaks[i];
		sum += numWeaks[i];
	}
	in.close();

	// Allocate the data.
	weaks = new WeakClassifierHoG*[sum];

	// The current weak classifier.
	WeakClassifierHoG **cur = weaks;

	// Some parameters used.
	// z is unused... I don't know...
	int offsetW, offsetH, w, h;
	float min, max, z;
	float histogram[100];
	Feature reference;
	reference.Resize(36);

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

			// Read in all the parameters.
			in >> offsetH;
			in >> offsetW;
			in >> h;
			in >> w;
			in >> z;
			in >> min;
			in >> max;
			for (int k = 0; k < 36; k++) {
				in >> reference.data[k];
			}
			for (int k = 0; k < 100; k++) {
				in >> histogram[k];
			}

			// Initialize the weak classifier.
			*cur = new WeakClassifierHoG(reference, min, max, offsetW, offsetH, w, h, histogram);

			// Read in the threshold for the last weak classifier.
			if (j == numWeaks[i] - 1) {
				in >> threshold[i];
			}

			// Move the cur pointer.
			cur++;
		}

		in.close();
	}
	printf("Reading process completed!\n");
}

AdaBoostClassifier::~AdaBoostClassifier() {

	int cur = 0;
	for (int i = 0; i < numStages; i++) {
		for (int j = 0; j < numWeaks[i]; j++) {
			delete weaks[cur];
			cur++;
		}
	}

	delete[] weaks;
	delete[] numWeaks;
	delete[] threshold;
}

bool AdaBoostClassifier::Classify(const IntegralImage *intImage, const Rect &roi, float scale) {
	int cur = 0;
	for (int stage = 0; stage < numStages; stage++) {
		float thre = 0.0f;
		for (int curWeak = 0; curWeak < numWeaks[stage]; curWeak++) {
			thre += weaks[cur]->EvaluateThre(intImage, roi, scale);
			cur++;
		}
		if (thre < threshold[stage]) {
			return false;
		}
	}
	return true;
}