#include "IntegralImage.h"

IntegralImage::IntegralImage(cv::Mat *img) : m_img(img) {
	width = img->size().width;
	height = img->size().height;
}

IntegralImage::~IntegralImage() {}

