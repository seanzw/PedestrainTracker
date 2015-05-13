#include "HoGFeature.h"

HoGFeature::HoGFeature(int offsetW, int offsetH, int w, int h)
	: m_offsetH(offsetH), m_offsetW(offsetW), m_width(w), m_height(h) {}

HoGFeature::~HoGFeature() {

}


bool HoGFeature::Extract(const IntegralImage *intImage, const Rect &roi, Feature *feature, float scale) const {
	// Remeber to resize the feature vector.
	feature->Resize(36);

	int scaledWidth = (int)(m_width * scale);
	int scaledHeight = (int)(m_height * scale);

	Rect subregion(
		roi.upper + (int)(m_offsetH * scale), 
		roi.left + (int)(m_offsetW * scale), 
		scaledWidth / 2,
		scaledHeight / 2);

	/** 
	 * The patch is like this:
	 *
	 * <------[scaledWith / 2]------^-------[scaledWidth - scaledWidth / 2]------>
	 * |							|											 |
	 * [scaledHeight / 2]			|											 |
	 * |							|											 |
	 * ^----------------------------^--------------------------------------------^
	 * |							|											 |
	 * [scaledHeight -				|											 |
	 *  scaledHeight / 2]			|											 |
	 * |							|											 |
	 * ^----------------------------^--------------------------------------------^
	 */


	// Upper left.
	intImage->GetSum(subregion, feature->data);

	// Upper right.
	subregion.left += subregion.width;
	subregion.width = scaledWidth - subregion.width;
	intImage->GetSum(subregion, feature->data + 9);

	// Down right.
	subregion.upper += subregion.height;
	subregion.height = scaledHeight - subregion.height;
	intImage->GetSum(subregion, feature->data + 27);

	// Down left.
	subregion.width = scaledWidth / 2;
	subregion.left -= subregion.width;
	intImage->GetSum(subregion, feature->data + 18);

	return true;
}