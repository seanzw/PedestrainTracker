/**
 * Connected Components.
 */

#ifndef CONNECTED_COMPONENTS_HEADER
#define CONNECTED_COMPONENTS_HEADER

#include "GlobalHeader.h"
#include "Pool.h"

#define MORPH_SIZE 2
#define MPRPH_ELEM 0	// Rect Morph

// Switch this to turn on/off debug image show.
// #define CCPDEBUG(...) cv::imshow(__VA_ARGS__)
// #define CCPWAIT cv::waitKey()
#define CCPDEBUG(...)
#define CCPWAIT

class ConnectedComponents {
public:
	static void Find(cv::Mat &binary, bool polyApprox, float invPerimScale);

	// Data
	// Contours: the contours we find.
	// elements: const element used in morphology transform.
	static std::vector<std::vector<cv::Point>> contours;
	static const cv::Mat element;
};

#endif