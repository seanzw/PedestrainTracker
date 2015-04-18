/**********************************************
 IntegralImage interface.
 Author: Zhengrong Wang.
 **********************************************/

#ifndef INTEGRAL_IMAGE_HEADER
#define INTEGRAL_IMAGE_HEADER

#include "GlobalHeader.h"

class IntegralImage {
public:
	IntegralImage(cv::Mat *img);
	virtual ~IntegralImage();

	virtual unsigned int GetSum(const cv::Rect &roi) const { return 0; }

	// Used in HoG integral image.
	virtual void GetSum(const cv::Rect &roi, float *result) const {}

	// The original image.
	cv::Mat *m_img;
	int width, height;
};


#endif