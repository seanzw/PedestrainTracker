/**
 * Sample around a few (detection, target) pairs.
 *
 * @author Zhengrong Wang
 */

#ifndef MULTI_SAMPLER_HEADER
#define MULTI_SAMPLER_HEADER

#include "GlobalHeader.h"
#include "Options.h"
#include "Geometry.h"
#include "Pool.h"

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

	/**
	 * Iterator for target i.
	 * Since we won't modify the sample, we only provide const iterator.
	 * Here the const_iterator is different from C++11.
	 * In C++11: const_iterator = (Rect * const);
	 * Here: const_iterator = (Rect const *) = (const Rect *);
	 */
	class const_iterator {
	public:
		typedef const_iterator self_type;
		typedef const Rect value_type;
		typedef const Rect& reference;
		typedef const Rect* pointer;
		typedef int difference_type;
		typedef std::input_iterator_tag iterator_category;
		const_iterator(pointer p, int t, bool *m) : ptr(p), target(t), mask(m) {}
		self_type operator++() { 
			self_type old = *this;
			while (!mask[++i]) {
				ptr++;
			}
			return old;
		}
		reference operator*() { return *ptr; }
		pointer operator->() { return ptr; }
		bool operator==(const self_type& rhs) { return ptr == rhs.ptr; }
		bool operator!=(const self_type& rhs) { return ptr != rhs.ptr; }
		int GetTarget() { return (target == (i / MULTI_SAMPLER_SAMPLES)) ? 1 : -1; }
	private:
		pointer ptr;
		bool *mask;
		int target;
		int i;
	};

	const_iterator begin(int target) const { return const_iterator(&samples[0], target, mask); }
	const_iterator end() const { return const_iterator(&samples[0] + samples.size, -1, mask); }

protected:

	int capacity;

	/**
	 * Samples pool.
	 * 1D array: (Capacity + 1) * MULTI_SAMPLER_SAMPLES
	 * [0, capacity * MULTI_SAMPLER_SAMPLES - 1] : samples around targets.
	 * [capacity * MULTI_SAMPLER_SAMPLES, (capacity + 1) * MULTI_SAMPLER_SAMPLES] : samples for background.
	 */
	Pool<Rect> samples;
	bool *mask;

	static std::default_random_engine generator;
	std::normal_distribution<float> gaussianWidth;
	std::normal_distribution<float> gaussianHeight;

};

#endif