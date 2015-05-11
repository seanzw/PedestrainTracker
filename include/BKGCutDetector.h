/**
 * Use background cutting to detect pedestrain,
 * @author Zhengrong Wang.
 */

#ifndef BKGCUTDETECTOR_HEADER
#define BKGCUTDETECTOR_HEADER

// Switch this to turn on/off debug image show.
// #define BKGDEBUG(...) cv::imshow(__VA_ARGS__)
// #define BKGWAIT cv::waitKey()
#define BKGDEBUG(...)
#define BKGWAIT

#include "ImageDetector.h"
#include "ConnectedComponents.h"

class BKGCutDetector : public ImageDetector {
public:
	// Constructor.
	BKGCutDetector(IntegralImage *i, Classifier *c, const Options &op);

	bool Detect(const cv::Mat &img, 
		const cv::Point &origin,
		bool isMerge = true,
		const cv::Mat &bkg = defaultBackground
		);


protected:
	// Some options.
	feat binaryThre;
	double invPerimeterThre;
	float minAreaRatio;
	float maxAreaRatio;

	/**
	 * Given a rectangle and the size of the original image,
	 * expand the rectangle by edge_width.
	 * @param x: the x coordinates of the upper-left corner.
	 * @param y: the y coordinates of the upper-left corner.
	 * @param roi_h: the height of the rectangle.
	 * @param roi_w: the width of the rectangle.
	 * @param edge_width: how many pixels you want to expand?
	 * @param height, width: the size of the original image.
	 * @return a structure rect.
	 */
	rect CreateROI(int x, int y, int roi_h, int roi_w, int edge_width, int height, int width);
};

#endif