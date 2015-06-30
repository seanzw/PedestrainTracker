#include "RGIIntegralImage.h"

RGIIntegralImage::RGIIntegralImage(const cv::Mat &img) : IntegralImage(img) {
	intImage = new unsigned int[width * height * NUM_RGI_BINS];
	step = width * NUM_RGI_BINS;
	gray = cv::Mat(width, height, CV_8UC1);
}

RGIIntegralImage::RGIIntegralImage(int w, int h) : IntegralImage(w, h) {
	intImage = new unsigned int[width * height * NUM_RGI_BINS];
	step = width * NUM_RGI_BINS;
	gray = cv::Mat(width, height, CV_8UC1);
}

RGIIntegralImage::~RGIIntegralImage() {
	if (intImage != NULL) {
		delete[] intImage;
	}
	gray.release();
}

void RGIIntegralImage::CalculateInt(const cv::Mat &img) {

	// First transfer the color image to grayscale to get intensity.
	cv::cvtColor(img, gray, cv::COLOR_RGB2GRAY);

	// Clear the first row of integral image.
	memset((void *)intImage, 0, sizeof(unsigned int) * step);

	unsigned int sumRow[NUM_RGI_BINS];

	// Get the pointer to image data.
	uchar *imgData = img.data;
	uchar *grayData = img.data;

	int imgStep = img.step[0];
	int grayStep = gray.step[0];

	unsigned int curIntPos = 0, curImgPos = 0, curGrayPos = 0;
	int bin;

	// Calculate the first row.
	for (int col = 0; col < width; col++) {

		// Clear the sum of the row.
		memset((void *)sumRow, 0, sizeof(unsigned int) * NUM_RGI_BINS);
		
		// R channel.
		bin = FindBin(imgData[curImgPos]);
		sumRow[bin]++;

		// G channel.
		bin = FindBin(imgData[curImgPos + 1]);
		sumRow[bin + 3]++;

		// I channel.
		bin = FindBin(grayData[curGrayPos]);
		sumRow[bin + 6]++;

		// Set the integral image.
		memcpy((void *)(&intImage[curIntPos]), (void *)sumRow, sizeof(unsigned int) * NUM_RGI_BINS);

		// Update the position.
		curImgPos += 3;
		curGrayPos += 1;
		curIntPos += 9;

	}

	for (int row = 1; row < img.size().height; row++) {

		curIntPos	= row * step;
		curImgPos	= row * imgStep;
		curGrayPos	= row * grayStep;

		// Copy the last row here.
		memcpy((void *)&intImage[curIntPos], (void *)&intImage[curIntPos - step], sizeof(unsigned int) * step);

		// Clear the sum of the row.
		memset((void *)sumRow, 0, sizeof(unsigned int) * NUM_RGI_BINS);

		for (int col = 0; col < img.size().width; col++) {

			// R channel.
			bin = FindBin(imgData[curImgPos]);
			sumRow[bin]++;

			// G channel.
			bin = FindBin(imgData[curImgPos + 1]);
			sumRow[bin + 3]++;

			// I channel.
			bin = FindBin(grayData[curGrayPos]);
			sumRow[bin + 6]++;

			// Set the integral image.
			for (int i = 0; i < NUM_RGI_BINS; i++) {
				intImage[curIntPos + i] += sumRow[i];
			}

			// Update the position.
			curImgPos += 3;
			curGrayPos += 1;
			curIntPos += 9;
		}
	}
}

void RGIIntegralImage::GetSum(const Rect &roi, float *result) const {

	unsigned int *originPtr = &intImage[roi.upper * step + roi.left * NUM_RGI_BINS];

	int down = (roi.height - 1) * step;
	int right = roi.width * NUM_RGI_BINS;

	// Get the bins.
	for (int bin = 0; bin < NUM_RGI_BINS; bin++) {
		result[bin] = (float)(originPtr[0] + originPtr[down + right] - originPtr[down] - originPtr[right]);
		originPtr++;
	}
}