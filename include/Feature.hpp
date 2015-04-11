/*************************************
 Feature class.
 Basically it's just a vector.
 Author: Zhengrong Wang
 *************************************/

#ifndef FEATURE_HEADER
#define FEATURE_HEADER

#include <string.h>
#include "GlobalHeader.h"

class Feature {
};

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

#endif