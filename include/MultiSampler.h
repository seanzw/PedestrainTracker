/**
 * Sample around a few (detection, target) pairs.
 *
 * @author Zhengrong Wang
 */

#ifndef MULTI_SAMPLER_HEADER
#define MULTI_SAMPLER_HEADER

#include "GlobalHeader.h"
#include "TargetsFreeList.h"

// Const number of samples per targets.
#define MULTI_SAMPLER_SAMPLES 5

class MultiSampler {
public:
	MultiSampler(const Options &opts);
	~MultiSampler();

	/**
	 * Draw samples around the targets and detection pair.
	 * @param matchDets		Matched pairs.
	 * @param detections	Detections.
	 */
	void Sample(const Pool<int> &matchDets, const Pool<Rect> &detections, const Size &imgSize);

private:

	int capacity;

	/**
	 * Samples pool.
	 * 2D array: (Capacity + 1) by MULTI_SAMPLER_SAMPLES
	 * [0, capacity - 1] : samples around targets.
	 * [capacity] : samples for background.
	 */
	Pool<Pool<Rect>> samples;

	static std::default_random_engine generator;
	std::normal_distribution<float> gaussianWidth;
	std::normal_distribution<float> gaussianHeight;

};

#endif