#include "MultiSampler.h"

std::default_random_engine MultiSampler::generator;

MultiSampler::MultiSampler(const Options &opts) {

	capacity = opts.targetsFreeListCapacity;
	
	// Set the samples pool as (capacity + 1) rows.
	samples = Pool<Rect>((opts.targetsFreeListCapacity + 1) * MULTI_SAMPLER_SAMPLES);
	mask = new bool[samples.size];
}

MultiSampler::~MultiSampler() {

}

void MultiSampler::Sample(const Pool<int> &matchDets,
	const Pool<Rect> &dets, const Size &imgSize) {

	// How many samples we have already drawn for background?
	int numBKGSamples = 0;

	for (int i = 0; i < matchDets.size; i++) {

		// This target has no matched detection.
		// Simply pass it.
		if (matchDets[i] == -1) {
			// Set the mask to false.
			for (int j = i * MULTI_SAMPLER_SAMPLES; j < (i + 1) * MULTI_SAMPLER_SAMPLES; j++) {
				mask[j] = false;
			}
			continue;
		}

		// Set the gaussian noise for width and height.
		Rect det = dets[matchDets[i]];
		gaussianWidth = std::normal_distribution<float>(det.width * 0.125f, det.width * 0.125f);
		gaussianHeight = std::normal_distribution<float>(det.height * 0.125f, det.height * 0.125f);

		// Sample for the positive samples.
		for (int j = 0; j < MULTI_SAMPLER_SAMPLES; j++) {

			// Calculate the offset.
			int offset = i * MULTI_SAMPLER_SAMPLES + j;

			// Set the mask.
			mask[offset] = true;

			samples[offset].height = det.height;
			samples[offset].width = det.width;

			// Sample until it's inside the image.
			while (true) {
				int offsetHeight = (int)gaussianHeight(generator);
				int offsetWidth = (int)gaussianWidth(generator);
				samples[offset].left = det.left + offsetWidth;
				samples[offset].upper = det.upper + offsetHeight;
				if (imgSize.IsIn(samples[offset])) {
					break;
				}
			}
		} // Finish sampling for positive.

		// Sampling for background.
		if (numBKGSamples < MULTI_SAMPLER_SAMPLES) {

			int offset = capacity * MULTI_SAMPLER_SAMPLES + numBKGSamples;
			mask[offset] = true;

			// Set the size.
			samples[offset].height = det.height;
			samples[offset].width = det.width;

			// Sample until it's inside the image.
			while (true) {
				int offsetHeight = (int)gaussianHeight(generator);
				int offsetWidth = (int)gaussianWidth(generator);
				samples[offset].left = det.left + offsetWidth;
				samples[offset].upper = det.upper + offsetHeight;
				if (imgSize.IsIn(samples[offset])) {
					break;
				}
			}

			numBKGSamples++;
		}
	}
}

