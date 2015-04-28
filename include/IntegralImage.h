/**
 * @author Zhengrong Wang
 */

#ifndef INTEGRAL_IMAGE_HEADER
#define INTEGRAL_IMAGE_HEADER

#include "GlobalHeader.h"
#include "Geometry.h"

class IntegralImage {
public:
	IntegralImage(const cv::Mat &img);
	IntegralImage(int w, int h) : width(w), height(h) {}
	virtual ~IntegralImage();

	// Calculate the integral image.
	// Should be called before GetSum.
	virtual void CalculateInt(const cv::Mat &img) = 0;

	virtual unsigned int GetSum(const Rect &roi) const { return 0; }

	// Used in HoG integral image.
	virtual void GetSum(const Rect &roi, float *result) const {}

	int width, height;
};


#endif