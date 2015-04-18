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

HoGIntegralImage::HoGIntegralImage(cv::Mat *img) : IntegralImage(img) {
	// Initialize the integral image.
	intImage = new float[width * height * NUM_HOG_BINS];

	m_step = width * NUM_HOG_BINS;

	CalculateInt();
}

HoGIntegralImage::~HoGIntegralImage() {
	if (intImage != NULL) {
		delete[] intImage;
	}
}

void HoGIntegralImage::CalculateInt() {
	// Clear the first row of integral image.
	memset((void *)intImage, 0, sizeof(float) * m_step);

	// Get the pointer to image data.
	uchar *imgData = m_img->data;

	int imgStep = m_img->step1();

	unsigned int curIntPos, curImgPos;

	for (int row = 1; row < height - 1; row++) {

		curIntPos = row * m_step;
		curImgPos = row * imgStep;

		// Copy the last row here.
		memcpy((void *)&intImage[curIntPos], (void *)&intImage[curIntPos - m_step], sizeof(float) * m_step);

		for (int col = 1; col < width - 1; col++) {

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

			for (int bin = 0; bin < NUM_HOG_BINS; bin++) {
				intImage[curIntPos + bin] += intImage[curIntPos - NUM_HOG_BINS + bin];
			}

			intImage[curIntPos + direction] += magnitude;
		}
	}
}

// Get the result.
void HoGIntegralImage::GetSum(const cv::Rect &roi, float *result) const {

	// Upper left origin.
	int originX = roi.x;
	int originY = roi.y;

	float *originPtr = &intImage[originY * m_step + originX * NUM_HOG_BINS];

	int down = roi.height * m_step;

	// Get the bins
	for (int bin = 0; bin < NUM_HOG_BINS; bin++) {
		result[bin] = originPtr[0] + originPtr[down + roi.width] - originPtr[down] - originPtr[roi.width];
		originPtr++;
	}
}

