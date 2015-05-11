/**
 * There are basically two kinds of detectors.
 * ImageDetector: detects pedestrains in an image.
 * VideoDetector: detects pedestrains in a video.
 */

#ifndef IMAGE_DETECTOR_HEADER
#define IMAGE_DETECTOR_HEADER

// #include "FeatureExtractor.h"
#include "Classifier.h"
#include "Pool.h"

struct rect
{
	int x1;
	int y1;
	int x2;
	int y2;
	int re;
	int count;
	rect(int _x1 = 0, int _y1 = 0, int _x2 = 0, int _y2 = 0)
		: x1(_x1), y1(_y1), x2(_x2), y2(_y2), re(-1), count(1) {}
};

// Use some specific feature and classifier to detect pedestrain.
// Apply this directly on the whole image.
class ImageDetector {
public:
	ImageDetector(IntegralImage *i, Classifier *c, const Options &op);
	
	/**
	 * Detect, return the resutl in a vector.
	 * Notice that sometimes we only want to detect in a subregion of the original
	 * image, in such case we have to give the origin parameter, which is the coordinates
	 * of the upper-left corner in the original image.
	 * @param img: the image we need to detect.
	 * @param origin: the coordinates of the img's upper-left corner in its parents (if any)
	 * @param bkg: unused here, for BKGCutDetector.
	 * @param isMerge: merge the detection with accumulated evidence.
	 * @return true if everything is fine.
	 */
	virtual bool Detect(const cv::Mat &img, 
		const cv::Point &origin = cv::Point(0, 0),
		bool isMerge = true,
		const cv::Mat &bkg = defaultBackground
		);

	void DrawDetection(cv::Mat &img);

	// Clear all the detections.
	void Clear();

	// Detections.
	Pool<rect> dets;

protected:
	IntegralImage *intImage;
	Classifier *classifier;
	feat scaleMin;
	feat scaleMax;
	feat scaleStep;
	int slideStep;
	int evidence;
	int modelHeight;
	int modelWidth;

	
	// Temp pool used in merge.
	Pool<rect> temp;

	// Some helper functions.
	// Is these two detection the same one?
	bool IsEqual(const rect &r1, const rect &r2) const;
	bool IsOverlap(const rect &r, const rect &t) const;
};

void DrawDetection(cv::Mat &img, Pool<rect> &dets);


#endif
