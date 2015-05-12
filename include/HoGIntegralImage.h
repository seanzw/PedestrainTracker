/**
 * The HoG integral image, use 9 bins.
 * @author Zhengrong Wang.
 */

#ifndef HOG_INTEGRAL_IMAGE_HEADER
#define HOG_INTEGRAL_IMAGE_HEADER

#define NUM_HOG_BINS 9

#include "IntegralImage.h"

class HoGIntegralImage : public IntegralImage {
public:
	// img should be CV_8UC1.
	HoGIntegralImage(const cv::Mat &img);
	HoGIntegralImage(int w, int h);
	~HoGIntegralImage();

	// Calculate the integral image.
	// Should be called if the img changed.
	virtual void CalculateInt(const cv::Mat &img);

	virtual void Dump(const char *filename) const;

	virtual void GetSum(const Rect &roi, float *result) const;

private:
	// Data.
	float *intImage;

	// Step.
	int m_step;

	static const float xVector[9];
	static const float yVector[9];

	// Some helper function.
	inline int Direct(float x, float y) {
		float max = 0.0;
		int ret = 0;

		// Try to avoid float problem.
		if (y == 0.0f)
			y = 0.01f;
		for (int i = 0; i < 9; i++) {
			float proj = fabsf(x * xVector[i] + y * yVector[i]);
			if (proj > max) {
				max = proj;
				ret = i;
			}
		}
		return ret;
	}
};

#endif

