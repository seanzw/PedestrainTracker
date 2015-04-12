#include "ConnectedComponents.h"

std::vector<std::vector<cv::Point>> ConnectedComponents::contours;
const cv::Mat ConnectedComponents::element = cv::getStructuringElement(
	MPRPH_ELEM, 
	cv::Size(2 * MORPH_SIZE + 1, 2 * MORPH_SIZE + 1), 
	cv::Point(MORPH_SIZE, MORPH_SIZE));

void ConnectedComponents::Find(cv::Mat &binary, bool polyApprox, float invPerimScale) {

	CCPDEBUG("binary", binary);
	CCPWAIT;

	// Clean the binary image.
	cv::morphologyEx(binary, binary, cv::MORPH_OPEN, element);
	cv::morphologyEx(binary, binary, cv::MORPH_CLOSE, element);

	CCPDEBUG("binary", binary);
	CCPWAIT;

	// Find contours.
	cv::findContours(binary, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	for (int i = contours.size() - 1; i >= 0; i--) {

		// Get the area of the contours.
		double area = cv::contourArea(contours[i]);
		cv::Rect box = cv::boundingRect(contours[i]);

		cv::rectangle(binary, box, cv::Scalar(255));
		CCPDEBUG("block", binary);
		CCPWAIT;

		double areaThre = (box.width + 90) * (box.height + 90) * invPerimScale;

		if (area < areaThre) {
			// This contours is not big enough.
			contours[i].clear();
		}
		else {
			// Approximate the contours with polygon or convex hull.
			if (polyApprox)
				cv::approxPolyDP(contours[i], contours[i], 2, true);
			else
				cv::convexHull(contours[i], contours[i], true);
		}

	}

}