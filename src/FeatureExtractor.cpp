#include "FeatureExtractor.h"

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


HoGExtractor::HoGExtractor(int width, int height) {
	// Initialize integral images.
	integrals = new feat[width * height * 9];
	bins = new feat[width * height * 9];

	widthStep = width * 9;
}

void HoGExtractor::Preprocess(const cv::Mat& img) {

	uchar *data = img.data;
	int height = img.size().height;
	int width = img.size().width;
	int step = img.step1();

	memset((void *)integrals, 0, sizeof(feat) * widthStep);
	memset((void *)bins, 0, sizeof(feat) * widthStep);

	for (int i = 1; i < height - 1; i++) {
		for (int j = 1; j < width - 1; j++) {
			feat tempX = (feat)data[(i - 1)*step + (j + 1)] - (feat)data[(i - 1)*step + (j - 1)]
				+ 2 * (feat)data[(i)*step + (j + 1)] - 2 * (feat)data[(i)*step + (j - 1)]
				+ (feat)data[(i + 1)*step + (j + 1)] - (feat)data[(i + 1)*step + (j - 1)];

			feat tempY = (feat)data[(i + 1)*step + (j - 1)] - (feat)data[(i - 1)*step + (j - 1)]
				+ 2 * (feat)data[(i + 1)*step + (j)] - 2 * (feat)data[(i - 1)*step + (j)]
				+ (feat)data[(i + 1)*step + (j + 1)] - (feat)data[(i - 1)*step + (j + 1)];

			int direction = Direct(tempX, tempY);
			feat temp = SQRT(tempX*tempX + tempY*tempY);

			int index = (i - 1) * widthStep + (j - 1) * 9;

			if (i == 1) {
				bins[index + direction] = temp;
			}
			else {
				memcpy((void *)(bins + index), (void *)(bins + index - widthStep), sizeof(feat) * 9);
				bins[index + direction] += temp;
			}

			memcpy((void *)(integrals + index), (void *)(bins + index), sizeof(feat) * 9);
			if (j > 1) {
				for (int k = 0; k < 9; k++) {
					integrals[index + k] += integrals[index + k - 9];
				}
			}
		}
	}
}

void HoGExtractor::Extract(int width, int height, int i, int j, Feature *feature) {

	// Remeber to resize the feature vector.
	feature->Resize(36);

	int i0 = i - 1;
	int j0 = (j - 1) * 9;
	int i1 = i + height / 2 - 1;
	int j1 = (j + width / 2 - 1) * 9;
	int i2 = i + height - 1;
	int j2 = (j + width - 1) * 9;
	for (int k = 0; k < 9; k++) {
		feat *row0 = integrals + i0 * widthStep;
		feat *row1 = integrals + i1 * widthStep;
		feat *row2 = integrals + i2 * widthStep;
		feature->data[k] = row1[j1 + k] + row0[j0 + k] - row1[j0 + k] - row0[j1 + k];
		feature->data[k + 9] = row1[j2 + k] + row0[j1 + k] - row1[j1 + k] - row0[j2 + k];
		feature->data[k + 18] = row2[j1 + k] + row1[j0 + k] - row1[j1 + k] - row2[j0 + k];
		feature->data[k + 27] = row2[j2 + k] + row1[j1 + k] - row1[j2 + k] - row2[j1 + k];
	}
}

HoGExtractor::~HoGExtractor() {
	delete[] integrals;
	delete[] bins;
}