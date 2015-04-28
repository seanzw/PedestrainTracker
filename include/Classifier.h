/**
 * This is a binary classifier.
 * @author Zhengrong Wang.
 */

#ifndef CLASSIFIER_HEADER
#define CLASSIFIER_HEADER

// #include "FeatureExtractor.h"
#include "IntegralImage.h"

class Classifier {
public:
	Classifier();
	virtual ~Classifier();

	virtual int Classify(const IntegralImage *intImage, const Rect &roi, float scale = 1.0f) {
		return -1;
	}

	virtual float Evaluate(const IntegralImage *intImage, const Rect &roi, float scale = 1.0f) {
		return FLT_MIN;
	}
};

/*
// Define the structure of features.
// Notice that the only the last weak classifier
// in a stage use the threshold field.
// Keep it in the structure is for memory alignment.
#define H_OFFSET		0
#define W_OFFSET		1
#define HEIGHT_OFFSET	2
#define WIDTH_OFFSET	3
#define Z_OFFSET		4
#define MIN_OFFSET		5
#define MAX_OFFSET		6
#define THRE_OFFSET		7
#define PROJ_OFFSET		8
#define HISTO_OFFSET	44
#define NUM_PROJ		36
#define NUM_HISTO		100

// Total size of a weak classifier.
#define SIZE_WEAK		144

/*************************************
The structure is like this:

struct WeakClassifier {
feat h;
feat w;
feat height;
feat width;
feat z;
feat min;
feat max;
feat threshold;
feat projection[36];
feat histogram[100];
};

****************************************************/

/*
class AdaBoostClassifier : public Classifier {
public:
	// Build the AdaBoost Classifier from files.
	AdaBoostClassifier(const char *filepath);

	~AdaBoostClassifier();

	// Classify the patch.
	// @param x: the x coordinate of upperleft corner.
	// @param y: the y coordinate of upperleft corner.
	// @param featureExt: the Feature Extractor.
	bool Classify(int x, int y, feat scale, FeatureExtractor &featureExt);

private:
	// Data.
	int numStages;
	int *numWeaks;
	feat *data;

	inline int District(feat value, feat min, feat max) const {
		if (value <= min) return 0;
		if (value >= max) return 99;

		feat step = (max - min) * 0.01;

		for (int i = 0; i < 100; i++)
			if (value >= min + step * i - 0.000001 && value <= min + step * (i + 1) + 0.000001)
				return i;		
	}

	// A Feature instance to store the feature.
	Feature feature;
};

*/
#endif
