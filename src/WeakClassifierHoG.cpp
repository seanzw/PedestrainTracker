#include "WeakClassifierHoG.h"

WeakClassifierHoG::WeakClassifierHoG(const Feature &reference, float min, float max,
	int offsetW, int offsetH, int w, int h,
	float *histogram) {

	// Construct the HoGFeature.
	hogFeature = new HoGFeature(offsetW, offsetH, w, h);

	this->min = min;
	this->max = max;

	// Copy the histogram.
	memcpy((void *)this->histogram, (void *)histogram, sizeof(float) * 100);

	// Copy the reference feature.
	this->reference.Resize(36);
	memcpy((void *)this->reference.data, (void *)reference.data, sizeof(float) * 36);

	// Prepare the result feature.
	this->result.Resize(36);
}

WeakClassifierHoG::~WeakClassifierHoG() {
	delete hogFeature;
}

float WeakClassifierHoG::Evaluate(const IntegralImage *intImage, const cv::Rect &roi, float scale) {
	
	// Get the feature.
	hogFeature->Extract(intImage, roi, &result, scale);

	float sum = 0.0;
	for (int k = 0; k < 36; k++)
		sum += result.data[k];

	// Normalize the result.
	if (sum > 120.0) {
		float invSum = 1.0 / sum;
		for (int k = 0; k < 36; k++)
			result.data[k] *= invSum;
	}
	else {
		return FLT_MIN;
	}

	// Projection.
	float projValue = 0.0;
	for (int k = 0; k < 36; k++) {
		projValue += reference.data[k] * result.data[k];
	}

	return projValue;
}

float WeakClassifierHoG::EvaluateThre(const IntegralImage *intImage, const cv::Rect &roi, float scale) {
	float proj = Evaluate(intImage, roi, scale);

	// Find the district.
	int district = District(proj, min, max);
	return histogram[district];
}

bool WeakClassifierHoG::Update(const IntegralImage *intImage, const cv::Rect &roi, bool target) {
	return true;
}
