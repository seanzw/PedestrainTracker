/**
 * Sampler interface.
 * This is used to sample neg and pos examples.
 * Given a positive sample, it samples numNegSamples negative samples around it.
 * @author Zhengrong Wang, Hsienyu Meng.
 */

#ifndef SAMPLER_HEADER
#define SAMPLER_HEADER

#include "Geometry.h"

class SingleSampler {
public:
	SingleSampler(int numPos, int numNeg);
	virtual ~SingleSampler();

	int GetNumPos() const;
	int GetNumNeg() const;

	const Rect &GetPosSample(int index) const;
	const Rect &GetNegSample(int index) const;

	// Sample around the positive sample.
	virtual void Sample(const Rect &pos, const Size &imgSize);

	void DrawSamples(cv::Mat &img, const cv::Scalar &posColor, const cv::Scalar &negColor) const;

protected:
	int numNegSamples;
	int numPosSamples;
	Rect *negSamples;
	Rect *posSamples;

	int curPosSample;
	int curNegSample;

	static std::default_random_engine generator;
	std::normal_distribution<float> gaussianWidth;
	std::normal_distribution<float> gaussianHeight;
};

#endif