#include "BKGCutDetector.h"

BKGCutDetector::BKGCutDetector(IntegralImage *i, Classifier *c, Options &op)
	: ImageDetector(i, c, op) {
	binaryThre = op.binaryThre;
	invPerimeterThre = op.invPerimeterRatio;
	minAreaRatio = op.minAreaRatio;
	maxAreaRatio = op.maxAreaRatio;
}

bool BKGCutDetector::Detect(const cv::Mat &img,
	const cv::Point &origin,
	bool isMerge,
	const cv::Mat &bkg
	) {

	cv::Mat imgBlur(img.size(), CV_8UC1);

	// Blur the image with Gaussian filter.
	cv::GaussianBlur(img, imgBlur, cv::Size(3, 3), 1.0);

	// The area of the img;
	float imgArea = img.size().width * img.size().height;

	// Calculate the difference.
	cv::Mat difference(imgBlur.size(), CV_32FC1);
	cv::absdiff(imgBlur, bkg, difference);

	// Get the binary image.
	cv::threshold(difference, difference, binaryThre, 255.0, cv::THRESH_BINARY);

	BKGDEBUG("difference", difference);
	BKGWAIT;

	// Find the connected components.
	ConnectedComponents::Find(difference, false, invPerimeterThre);

	for (int i = 0; i < ConnectedComponents::contours.size(); i++) {
		if (ConnectedComponents::contours[i].size() == 0)
			continue;

		// Get the bounding box;
		cv::Rect box = cv::boundingRect(ConnectedComponents::contours[i]);

		float boxArea = box.height * box.width;
		if (box.height > box.width * 2 &&
			boxArea <= maxAreaRatio * imgArea &&
			boxArea >= minAreaRatio * imgArea) {

			// Add directly.
			rect det = CreateROI(box.x, box.y, box.height, box.width, 10, img.size().height, img.size().width);
			dets.Push(det);
		}
		else {
			// Use classifier to judge.
			// Expand the ROI by 30 pixels to make sure that classifier will
			// find the pedestrain.
			rect det = CreateROI(box.x, box.y, box.height, box.width, 30, img.size().height, img.size().width);
			cv::Mat block = img(cv::Rect(det.x1, det.y1, det.x2 - det.x1, det.y2 - det.y1));

			BKGDEBUG("block", block);
			BKGWAIT;

			ImageDetector::Detect(block, cv::Point(det.x1, det.y1), isMerge);
		}
	}

	return true;
}

rect BKGCutDetector::CreateROI(int x, int y, int roi_h, int roi_w, int edge_width, int height, int width) {
	int roix, roiy, roih, roiw;
	roix = ((x - edge_width) > 0) ? (x - edge_width) : 1;
	roiy = ((y - edge_width) > 0) ? (y - edge_width) : 1;
	roih = ((roiy + roi_h + 2 * edge_width) > height) ? (height - roiy) : (roi_h + 2 * edge_width);
	roiw = ((roix + roi_w + 2 * edge_width) > width) ? (width - roix) : (roi_w + 2 * edge_width);
	rect roi(roix, roiy, roiw + roix, roih + roiy);
	return roi;
}