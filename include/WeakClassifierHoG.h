/*****************************************************
 A weak classifier using HoG.
 Given a roi in an image, finds the HoG
 in some subregion and project it to get some value.
 Author: Zhengrong Wang.
 ****************************************************/

#ifndef WEAKCLASSIFIER_HOG_HEADER
#define WEAKCLASSIFIER_HOG_HEADER

#include "WeakClassifier.h"
#include "HoGFeature.h"

class WeakClassifierHoG : public WeakClassifier {
public:

	// Construct a weak classifier.
	// @param reference: the reference for this weak classifier.
	// @param min, max: used to find the histogram.
	// @oaram histogram: histogram.
	// @param others: for the HoGFeature.
	WeakClassifierHoG(const Feature &reference, float min, float max,
		int offsetW, int offsetH, int w, int h, 
		float *histogram);
	~WeakClassifierHoG();

	bool Update(const IntegralImage *intImage, const cv::Rect &roi, bool target);

	// Return the dot product between the reference and result.
	float Evaluate(const IntegralImage *intImage, const cv::Rect &roi, float scale = 1.0f);

	// Return the increament in threshold.
	// Used in AdaBoosting classifier.
	float EvaluateThre(const IntegralImage *intImage, const cv::Rect &roi, float scale = 1.0f);

private:
	// A HoG feature.
	HoGFeature *hogFeature;

	// A reference HoG feature.
	Feature reference;

	// A Feature to store the result from HoG feature extractor.
	Feature result;

	float histogram[100];
	float min, max;

	// Find the district.
	inline int District(float value, float min, float max) const {
		if (value <= min) return 0;
		if (value >= max) return 99;

		float step = (max - min) * 0.01;

		for (int i = 0; i < 100; i++)
			if (value >= min + step * i - 0.000001 && value <= min + step * (i + 1) + 0.000001)
				return i;
	}


	// A 36 dimension classifier threshold.
	// ClassifierThreshold<36> classifier;
};

#endif