/**
 * The RGI histogram integral image.
 * 3 bins for each channel.
 *
 * @author Zhengrong Wang
 */

#ifndef RGI_INTEGRALIMAGE_HEADER
#define RGI_INTEGRALIMAGE_HEADER

#define NUM_RGI_BINS	9

#include "IntegralImage.h"

class RGIIntegralImage : public IntegralImage {
public:
	/**
	 * @param img	should be 8UC3, e.g. RGB image.
	 */
	RGIIntegralImage(const cv::Mat &img);
	RGIIntegralImage(int w, int h);
	~RGIIntegralImage();

	void CalculateInt(const cv::Mat &img);

	void GetSum(const Rect &roi, float *result) const;

private:

	unsigned int *intImage;

	cv::Mat gray;

	int step;

	/**
	 * The [0, 255] is divided as [0, 85], [86, 170] and [171, 255]
	 */
	inline int FindBin(uchar x) {
		if (x <= 85)
			return 0;
		if (x <= 170)
			return 1;
		return 2;
	}

};

#endif