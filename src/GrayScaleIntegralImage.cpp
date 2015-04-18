#include "GrayScaleIntegralImage.h"

GrayScaleIntegralImage::GrayScaleIntegralImage(cv::Mat *img) : IntegralImage(img) {
	// Initialize the integral image.
	intImage = new unsigned int[width * height];

	CalculateInt();
}

GrayScaleIntegralImage::~GrayScaleIntegralImage() {
	if (intImage != NULL) {
		delete[] intImage;
	}
}

unsigned int GrayScaleIntegralImage::GetSum(const cv::Rect &roi) const {
	
	// Upper left origin.
	int originX = roi.x;
	int originY = roi.y;

	unsigned int *originPtr = &intImage[originY * (width)+originX];

	int down = roi.height * width;

	return originPtr[0] + originPtr[down + roi.width] - originPtr[down] - originPtr[roi.width];
}



void GrayScaleIntegralImage::CalculateInt() {
	// Clear the integral image.
	memset((void *)intImage, 0, sizeof(unsigned int) * width * height);

	int step = m_img->step1();

	// The temp sum for each row.
	unsigned int sumRow;
	unsigned int curIntPos, curImgPos;

	// Get the first row.
	for (int col = 0; col < width; col++) {
		intImage[col] = m_img->data[col];
	}

	for (int row = 1; row < height; row++) {

		// Current row in integral image.
		curIntPos = (row + 1) * (width) + 1;
		curImgPos = row * step;

		sumRow = 0;

		for (int col = 0; col < width; col++) {
			sumRow += m_img->data[curImgPos];

			// Update the integral image.
			intImage[curIntPos] = intImage[curIntPos - width] + sumRow;

			// Update the position.
			curImgPos++;
			curIntPos++;
		}
	}
}
