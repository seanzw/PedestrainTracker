#include "MultiSampler.h"

std::default_random_engine MultiSampler::generator;

MultiSampler::MultiSampler(const Options &opts) {

	capacity = opts.targetsFreeListCapacity;
	
	// Set the samples pool as (capacity + 1) rows.
	samples = Pool<Pool<Rect>>(opts.targetsFreeListCapacity + 1);

	// MULTI_SAMPLER_SAMPLES samples for each targets and background.
	for (int i = 0; i < samples.size; i++) {
		samples[i] = Pool<Rect>(MULTI_SAMPLER_SAMPLES);
	}
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
		if (matchDets[i] == -1)
			continue;

		// Set the gaussian noise for width and height.
		Rect det = dets[matchDets[i]];
		gaussianWidth = std::normal_distribution<float>(det.width * 0.125f, det.width * 0.125f);
		gaussianHeight = std::normal_distribution<float>(det.height * 0.125f, det.height * 0.125f);

		// Sample for the positive samples.
		for (int j = 0; j < MULTI_SAMPLER_SAMPLES; j++) {

			// Set the size.
			samples[i][j].height = det.height;
			samples[i][j].width = det.width;

			// Sample until it's inside the image.
			while (true) {
				int offsetHeight = (int)gaussianHeight(generator);
				int offsetWidth = (int)gaussianWidth(generator);
				samples[i][j].left = det.left + offsetWidth;
				samples[i][j].upper = det.upper + offsetHeight;
				if (imgSize.IsIn(samples[i][j])) {
					break;
				}
			}
		} // Finish sampling for positive.

		// Sampling for background.
		if (numBKGSamples < MULTI_SAMPLER_SAMPLES) {

			// Set the size.
			samples[capacity][numBKGSamples].height = det.height;
			samples[capacity][numBKGSamples].width = det.width;

			// Sample until it's inside the image.
			while (true) {
				int offsetHeight = (int)gaussianHeight(generator);
				int offsetWidth = (int)gaussianWidth(generator);
				samples[capacity][numBKGSamples].left = det.left + offsetWidth;
				samples[capacity][numBKGSamples].upper = det.upper + offsetHeight;
				if (imgSize.IsIn(samples[capacity][numBKGSamples])) {
					break;
				}
			}

			numBKGSamples++;
		}
	}

}

