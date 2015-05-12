#include "HoGIntegralImage.h"

#define SINPI1 0.173648
#define COSPI1 0.984808
#define SINPI2 0.500000
#define COSPI2 0.866025
#define SINPI3 0.766044
#define COSPI3 0.642788
#define SINPI4 0.939693
#define COSPI4 0.342020
#define SINPI5 1.000000
#define COSPI5 0.000000
#define SINPI6 0.939693
#define COSPI6 -0.342020
#define SINPI7 0.766044
#define COSPI7 -0.642788
#define SINPI8 0.500000
#define COSPI8 -0.866025
#define SINPI9 0.173648
#define COSPI9 -0.984808

const float HoGIntegralImage::xVector[9] = { COSPI1, COSPI2, COSPI3, COSPI4, COSPI5, COSPI6, COSPI7, COSPI8, COSPI9 };
const float HoGIntegralImage::yVector[9] = { SINPI1, SINPI2, SINPI3, SINPI4, SINPI5, SINPI6, SINPI7, SINPI8, SINPI9 };

HoGIntegralImage::HoGIntegralImage(const cv::Mat &img) : IntegralImage(img) {
	// Initialize the integral image.
	intImage = new float[width * height * NUM_HOG_BINS];
	m_step = width * NUM_HOG_BINS;
}

HoGIntegralImage::HoGIntegralImage(int w, int h) : IntegralImage(w, h) {
	intImage = new float[width * height * NUM_HOG_BINS];
	m_step = width * NUM_HOG_BINS;
}

HoGIntegralImage::~HoGIntegralImage() {
	if (intImage != NULL) {
		delete[] intImage;
	}
}

void HoGIntegralImage::CalculateInt(const cv::Mat &img) {
	// Clear the integral image.
	memset((void *)intImage, 0, sizeof(float) * m_step * height);

	float sumRow[NUM_HOG_BINS];

	// Get the pointer to image data.
	uchar *imgData = img.data;

	int imgStep = img.step[0];

	unsigned int curIntPos, curImgPos;

	for (int row = 1; row < img.size().height - 1; row++) {

		curIntPos = row * m_step;
		curImgPos = row * imgStep;

		// Copy the last row here.
		memcpy((void *)&intImage[curIntPos], (void *)&intImage[curIntPos - m_step], sizeof(float) * m_step);

		// Clear the sum of the row.
		memset((void *)sumRow, 0, sizeof(float) * NUM_HOG_BINS);

		for (int col = 1; col < img.size().width - 1; col++) {

			curIntPos += NUM_HOG_BINS;
			curImgPos ++;

			// Calculate the gradiant.
			float gradX = 
				(float)imgData[curImgPos - imgStep + 1] -
				(float)imgData[curImgPos - imgStep - 1] +
				2 * (float)imgData[curImgPos + 1] -
				2 * (float)imgData[curImgPos - 1] +
				(float)imgData[curImgPos + imgStep + 1] -
				(float)imgData[curImgPos + imgStep - 1];

			float gradY = 
				(float)imgData[curImgPos + imgStep - 1] -
				(float)imgData[curImgPos - imgStep - 1] +
				2 * (float)imgData[curImgPos + imgStep] -
				2 * (float)imgData[curImgPos - imgStep] +
				(float)imgData[curImgPos + imgStep + 1] -
				(float)imgData[curImgPos - imgStep + 1];

			// Get the direction of the gradient.
			int direction = Direct(gradX, gradY);
			float magnitude = sqrtf(gradX * gradX + gradY * gradY);

			sumRow[direction] += magnitude;

			for (int bin = 0; bin < NUM_HOG_BINS; bin++) {
				intImage[curIntPos + bin] += sumRow[bin];
			}
		}
	}
}

// Get the result.
void HoGIntegralImage::GetSum(const Rect &roi, float *result) const {

	float *originPtr = &intImage[roi.upper * m_step + roi.left * NUM_HOG_BINS];

	int down = roi.height * m_step;
	int right = roi.width * NUM_HOG_BINS;

	// Get the bins.
	for (int bin = 0; bin < NUM_HOG_BINS; bin++) {
		result[bin] = originPtr[0] + originPtr[down + right] - originPtr[down] - originPtr[right];
		originPtr++;
	}
}

void HoGIntegralImage::Dump(const char *filename) const {
	std::ofstream dump(filename, std::ios::out);

	dump << std::fixed << std::setprecision(4);

	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			float *originPtr = &intImage[row * m_step + col * NUM_HOG_BINS];
			for (int i = 0; i < NUM_HOG_BINS; i++) {
				dump << originPtr[i] << ' ';
			}
			dump << std::endl;
		}
		dump << std::endl;
	}
}