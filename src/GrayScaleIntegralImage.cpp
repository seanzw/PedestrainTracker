#include "GrayScaleIntegralImage.h"

GrayScaleIntegralImage::GrayScaleIntegralImage(const cv::Mat &img) : IntegralImage(img) {
	// Initialize the integral image.
	intImage = new unsigned int[width * height];
}

GrayScaleIntegralImage::GrayScaleIntegralImage(int w, int h) : IntegralImage(w, h) {
	intImage = new unsigned int[width * height];
}

GrayScaleIntegralImage::~GrayScaleIntegralImage() {
	if (intImage != NULL) {
		delete[] intImage;
	}
}

unsigned int GrayScaleIntegralImage::GetSum(const Rect &roi) const {

	unsigned int *originPtr = &intImage[roi.upper * width + roi.left];

	int down = roi.height * width;

	return originPtr[0] + originPtr[down + roi.width] - originPtr[down] - originPtr[roi.width];
}



void GrayScaleIntegralImage::CalculateInt(const cv::Mat &img) {
	// Clear the integral image.
	memset((void *)intImage, 0, sizeof(unsigned int) * width * height);

	int step = img.step1();

	// The temp sum for each row.
	unsigned int sumRow;
	unsigned int curIntPos, curImgPos;

	// Get the first row.
	intImage[0] = (unsigned int)img.data[0];
	for (int col = 1; col < width; col++) {
		intImage[col] = img.data[col] + intImage[col - 1];
	}

	for (int row = 1; row < height; row++) {

		// Current row in integral image.
		curIntPos = row * width;
		curImgPos = row * step;

		sumRow = 0;

		for (int col = 0; col < width; col++) {
			sumRow += img.data[curImgPos];

			// Update the integral image.
			intImage[curIntPos] = intImage[curIntPos - width] + sumRow;

			// Update the position.
			curImgPos++;
			curIntPos++;
		}
	}
}
