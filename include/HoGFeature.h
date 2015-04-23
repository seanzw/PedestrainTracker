/************************************************
 HoGFeature class.
 Extract the HoG of subregion in the roi.
 Author: Zhengrong Wang.
 ************************************************/

#ifndef HOGFEATURE_HEADER
#define HOGFEATURE_HEADER

#include "IntegralImage.h"
#include "Feature.h"

class HoGFeature {
public:
	HoGFeature(int offsetW, int offsetH, int w, int h);
	~HoGFeature();

	bool Extract(const IntegralImage *intImage, const Rect &roi, Feature *feature, float scale = 1.0f) const;

private:
	const int m_offsetW, m_offsetH;
	const int m_width, m_height;
};

#endif