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
	void Sample(const std::vector<int> &matchDets, const Pool<Rect> &detections, const Size &imgSize);

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
		typedef std::vector<Rect>::const_pointer pointer;
		typedef int difference_type;
		typedef std::input_iterator_tag iterator_category;
		const_iterator(pointer p,
			int t,
			int l,
			bool *m) : ptr(p), target(t), mask(m), length(l) {}
		self_type operator++() {

			self_type old = *this;

			// First add one.
			i++;
			ptr++;

			// Check if this is a valid one.
			while (i < length && !mask[i]) {
				i++;
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
		int length;
	};

	const_iterator begin(int target) const { 
		const_iterator iter = const_iterator(&samples[0], target, samples.size(), mask);

		// If the first target is unused, we have to move iter.
		if (!mask[0]) {
			iter++;
		}

		return iter;
	}
	const_iterator end() const { return const_iterator(&samples[0] + samples.size(), -1, samples.size(), mask); }

protected:

	int capacity;

	/**
	 * Samples vector.
	 * 1D array: (Capacity + 1) * MULTI_SAMPLER_SAMPLES
	 * [0, capacity * MULTI_SAMPLER_SAMPLES - 1] : samples around targets.
	 * [capacity * MULTI_SAMPLER_SAMPLES, (capacity + 1) * MULTI_SAMPLER_SAMPLES] : samples for background.
	 */
	std::vector<Rect> samples;
	bool *mask;

	static std::default_random_engine generator;
	std::normal_distribution<float> gaussianWidth;
	std::normal_distribution<float> gaussianHeight;

};

#endif