#include "GrayScaleIntegralImage.h"

GrayScaleIntegralImage::GrayScaleIntegralImage(const cv::Mat &img) : IntegralImage(img) {
	// Initialize the integral image.
	intImage = new unsigned int[width * height];
	tiltIntImage = new unsigned int[(width + 1) * (height + 2)];
}

GrayScaleIntegralImage::GrayScaleIntegralImage(int w, int h) : IntegralImage(w, h) {
	intImage = new unsigned int[width * height];
}

GrayScaleIntegralImage::~GrayScaleIntegralImage() {
	if (intImage != NULL) {
		delete[] intImage;
		delete[] tiltIntImage;
	}
}

unsigned int GrayScaleIntegralImage::GetSum(const Rect &roi) const {

	unsigned int *originPtr = &intImage[roi.upper * width + roi.left];

	int down = (roi.height - 1) * width;

	return originPtr[0] + originPtr[down + roi.width - 1] - originPtr[down] - originPtr[roi.width - 1];
}



void GrayScaleIntegralImage::CalculateInt(const cv::Mat &img) {
	// Clear the integral image.
	memset((void *)intImage, 0, sizeof(unsigned int) * width * height);

	// step1(int i) = step[i] / channelBytes;
	// step1(int i = 0) = step[0] / channelBytes;
	int step = img.step[0];

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

void GrayScaleIntegralImage::CalculateTiltInt(const cv::Mat &img){

	img.step[0];


	//clear the integral image
	memset((unsigned int *)tiltIntImage, 0, sizeof(unsigned int) * (width+1)  * (height+2));
	
	//size_t step = 1;
	//calculate the numbers of bytes each channel occupies 
	int step = img.step[0];

	for (size_t row = 2; row!= (height+2); ++row){

		int curIntPos = row*(width+1);
		int curImgPos = (row - 2)*step;

		for (size_t col = 1; col != (width+1); ++col){

			intImage[curIntPos + col] = intImage[curIntPos - (width + 1) + (col - 1)] +
				intImage[curIntPos - (width + 1) + (col + 1)] -
				intImage[curIntPos - 2 * (width + 1) + col] +
				img.data[curImgPos + col - 1] +
				img.data[curImgPos - (width + 1) + col - 1];
		}

	}

}
