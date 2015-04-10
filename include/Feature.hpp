/*************************************
 Feature base class.
 Author: Zhengrong Wang
 *************************************/

#ifndef FEATURE_HEADER
#define FEATURE_HEADER

#include <string.h>

class Feature {
};

class HoGFeature : public Feature {
public:
	HoGFeature() {
		memset((void *)hogs, 0, 36 * sizeof(double));
	}

	HoGFeature(const double *src) {
		memcpy((void *)hogs, (const void *)src, 36 * sizeof(double));
	}

	// Overwrite the virtual function.
	void ToFloat(const double *dst) const {
		if (dst)
			memcpy((void *)dst, (const void *)hogs, 36 * sizeof(double));
	}

	// Data
	double hogs[36];
};

#endif