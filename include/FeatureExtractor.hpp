/*******************************************
 FeatureExtractor base class.
 *******************************************/

#ifndef FEATURE_EXTRACTOR_HEADER
#define FEATURE_EXTRACTOR_HEADER

#include "Feature.hpp"
#include "opencv\cv.h"
#include <vector>

typedef unsigned char uchar;

class FeatureExtractor {
public:
	// Interface.
	virtual void Extract(int width, int height, int i, int j, Feature *feature) = 0;

	// Some preprocess work, such as integral image.
	void Preprocess(const uchar *data, int width, int height) {}
};

// Extract a HoG feature.
// Use integral image to accelerate.
class HoGExtractor : public FeatureExtractor {
public:
	HoGExtractor(int width, int height) {
		// Initialize integral images.
		integrals = new double[width * height * 9];
		bins = new double[width * height * 9];

		widthStep = width * 9;
	}

	void Preprocess(const cv::Mat& img) {

		uchar *data = img.data;
		int height = img.size().height;
		int width = img.size().width;
		int step = img.step1();

		memset((void *)integrals, 0, sizeof(double) * widthStep);
		memset((void *)bins, 0, sizeof(double) * widthStep);

		for (int i = 1; i < height - 1; i++) {
			for (int j = 1; j < width - 1; j++) {
				double tempX = (double)data[(i - 1)*step + (j + 1)] - (double)data[(i - 1)*step + (j - 1)]
					+ 2 * (double)data[(i)*step + (j + 1)] - 2 * (double)data[(i)*step + (j - 1)]
					+ (double)data[(i + 1)*step + (j + 1)] - (double)data[(i + 1)*step + (j - 1)];

				double tempY = (double)data[(i + 1)*step + (j - 1)] - (double)data[(i - 1)*step + (j - 1)]
					+ 2 * (double)data[(i + 1)*step + (j)] - 2 * (double)data[(i - 1)*step + (j)]
					+ (double)data[(i + 1)*step + (j + 1)] - (double)data[(i - 1)*step + (j + 1)];

				int direction = Direct(tempX, tempY);
				double temp = sqrt(tempX*tempX + tempY*tempY);

				int index = (i - 1) * widthStep + (j - 1) * 9;

				if (i == 1) {
					bins[index + direction] = temp;
				} else {
					memcpy((void *)(bins + index), (void *)(bins + index - widthStep), sizeof(double) * 9);
					bins[index + direction] += temp;
				}

				memcpy((void *)(integrals + index), (void *)(bins + index), sizeof(double) * 9);
				if (j > 1) {
					for (int k = 0; k < 9; k++) {
						integrals[index + k] += integrals[index + k - 9];
					}
				}
			}
		}
		
		
	}

	// Use the integral image to extract HoG.
	// Must be called after Preprocess.
	void Extract(int width, int height, int i, int j, Feature *f) {
		HoGFeature *feature = (HoGFeature *)f;
		int i0 = i - 1;
		int j0 = (j - 1) * 9;
		int i1 = i + height / 2 - 1;
		int j1 = (j + width / 2 - 1) * 9;
		int i2 = i + height - 1;
		int j2 = (j + width - 1) * 9;
		for (int k = 0; k < 9; k++) {
			double *row0 = integrals + i0 * widthStep;
			double *row1 = integrals + i1 * widthStep;
			double *row2 = integrals + i2 * widthStep;
			feature->hogs[k]			= row1[j1 + k] + row0[j0 + k] - row1[j0 + k] - row0[j1 + k];
			feature->hogs[k + 9]		= row1[j2 + k] + row0[j1 + k] - row1[j1 + k] - row0[j2 + k];
			feature->hogs[k + 18]	= row2[j1 + k] + row1[j0 + k] - row1[j1 + k] - row2[j0 + k];
			feature->hogs[k + 27]	= row2[j2 + k] + row1[j1 + k] - row1[j2 + k] - row2[j1 + k];
		}
	}

	~HoGExtractor() {
		delete[] integrals;
		delete[] bins;
	}

private:
	static const float xVector[9];
	static const float yVector[9];

	inline int Direct(double x, double y) {
		double max = 0.0;
		int ret = 0;
		for (int i = 0; i < 9; i++) {
			double proj = fabs(x * xVector[i] + y * yVector[i]);
			if (proj > max) {
				max = proj;
				ret = i;
			}
		}
		return ret;
	}

	double *integrals;
	double *bins;
	int widthStep;
};

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

const float HoGExtractor::xVector[9] = { COSPI1, COSPI2, COSPI3, COSPI4, COSPI5, COSPI6, COSPI7, COSPI8, COSPI9 };
const float HoGExtractor::yVector[9] = { SINPI1, SINPI2, SINPI3, SINPI4, SINPI5, SINPI6, SINPI7, SINPI8, SINPI9 };


#endif