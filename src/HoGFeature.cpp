#include "HoGFeature.h"

HoGFeature::HoGFeature(int offsetW, int offsetH, int w, int h)
	: m_offsetH(offsetH), m_offsetW(offsetW), m_width(w), m_height(h) {}

HoGFeature::~HoGFeature() {

}


bool HoGFeature::Extract(const IntegralImage *intImage, const cv::Rect &roi, Feature *feature, float scale) const {
	// Remeber to resize the feature vector.
	feature->Resize(36);

	cv::Rect subregion(
		roi.x + (int)(m_offsetH * scale) - 1, 
		roi.y + (int)(m_offsetW * scale) - 1, 
		(int)(m_width * scale) / 2, 
		(int)(m_height * scale) / 2);

	// Upper left.
	intImage->GetSum(subregion, feature->data);

	// Upper right.
	subregion.y += subregion.width;
	intImage->GetSum(subregion, feature->data + 9);

	// Down right.
	subregion.x += subregion.height;
	intImage->GetSum(subregion, feature->data + 27);

	// Down left.
	subregion.y -= subregion.width;
	intImage->GetSum(subregion, feature->data + 18);

	return true;
}