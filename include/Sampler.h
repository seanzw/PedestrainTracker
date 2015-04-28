/**
 * Sampler class.
 * This is used to sample neg and pos examples.
 * Given a positive sample, it samples numNegSamples negative samples around it.
 * @author Zhengrong Wang, Hsienyu Meng.
 */

#ifndef SAMPLER_HEADER
#define SAMPLER_HEADER

#include "Geometry.h"

class Sampler {
public:
	Sampler(int num);
	virtual ~Sampler();

	// Return negSamples[index].
	const Rect &GetNegSample(int index) const;

	// Sample around the positive sample.
	virtual void Sample(const Rect &pos, const Size &imgSize);

protected:
	int numNegSamples;
	Rect *negSamples;
};

#endif