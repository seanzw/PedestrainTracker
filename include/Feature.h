/**
 * Basically it's just a vector.
 * @author Zhengrong Wang
 */

#ifndef FEATURE_HEADER
#define FEATURE_HEADER

#include <string.h>
#include "GlobalHeader.h"

class Feature {
public:
	Feature(int size = 0);
	~Feature();

	// Resize the feature.
	// Feature extractor should resize first to make sure
	// the feature instance is big enough.
	void Resize(int newSize);

	// Data is public.
	feat *data;
	int capacity;
};

/*

#define HOGDIMENSION 36

class HoGFeature : public Feature {
public:
	HoGFeature() {
		memset((void *)hogs, 0, HOGDIMENSION * sizeof(feat));
	}

	HoGFeature(const feat *src) {
		memcpy((void *)hogs, (const void *)src, HOGDIMENSION * sizeof(feat));
	}

	// Data
	feat hogs[HOGDIMENSION];
};
*/

#endif