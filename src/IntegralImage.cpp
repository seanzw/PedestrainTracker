#include "IntegralImage.h"

IntegralImage::IntegralImage(const cv::Mat &img) {
	width = img.size().width;
	height = img.size().height;
}

IntegralImage::~IntegralImage() {}