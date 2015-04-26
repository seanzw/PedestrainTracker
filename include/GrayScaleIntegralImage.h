/**********************************************
 GrayScaleIntegralImage class.
 The basic integral image. Integral the grayscale
 intensity. This can be used in Haar like feature.
 Author: Zhengrong Wang.
 **********************************************/

#ifndef GRAY_SCALE_INTEGRAL_IMAGE_HEADER
#define GRAY_SCALE_INTEGRAL_IMAGE_HEADER

#include "IntegralImage.h"

class GrayScaleIntegralImage : public IntegralImage {
public:
	// The img should be CV_8UC1
	GrayScaleIntegralImage(const cv::Mat &img);
	GrayScaleIntegralImage(int w, int h);
	~GrayScaleIntegralImage();

	// Calculate the integral image.
	// Should be called if the img changed.
	void CalculateInt(const cv::Mat &img);

	unsigned int GetSum(const Rect &roi) const;

private:
	// The integral image data.
	unsigned int *intImage;
};


#endif
