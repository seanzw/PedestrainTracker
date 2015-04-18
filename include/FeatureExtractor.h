/*******************************************
 FeatureExtractor base class.
 *******************************************/

#ifndef FEATURE_EXTRACTOR_HEADER
#define FEATURE_EXTRACTOR_HEADER

#include "Feature.h"

typedef unsigned char uchar;

class FeatureExtractor {
public:
	// Interface.
	virtual void Extract(int width, int height, int i, int j, Feature *feature) = 0;

	// Some preprocess work, such as integral image.
	virtual void Preprocess(const cv::Mat& img) {}
};

// Extract a HoG feature.
// Use integral image to accelerate.
class HoGExtractor : public FeatureExtractor {
public:
	HoGExtractor(int width, int height);

	void Preprocess(const cv::Mat& img);

	// Use the integral image to extract HoG.
	// Must be called after Preprocess.
	void Extract(int width, int height, int i, int j, Feature *feature);

	~HoGExtractor();

private:
	static const float xVector[9];
	static const float yVector[9];

	inline int Direct(feat x, feat y) {
		feat max = 0.0;
		int ret = 0;
		for (int i = 0; i < 9; i++) {
			feat proj = ABS(x * xVector[i] + y * yVector[i]);
			if (proj > max) {
				max = proj;
				ret = i;
			}
		}
		return ret;
	}

	feat *integrals;
	feat *bins;
	int widthStep;
};

#endif