/****************************************************
 This is the global header for pedestrain counting.
 Define some global constants.
 ****************************************************/

#ifndef GLOBAL_HEADER
#define GLOBAL_HEADER


#include <cstdio>
#include <math.h>
#include <fstream>
#include <string.h>
//#include "opencv\cv.h"
//#include "opencv\highgui.h"
#include "opencv2\opencv.hpp"
#include "opencv2\imgproc.hpp"

// Switch this to use double or float for features.
// #define DOUBLE_PRECISION

#ifdef DOUBLE_PRECISION
typedef double feat;
#define SQRT(x) sqrt(x)
#define ABS(x) fabs(x)
#else
typedef float feat;
#define SQRT(x) sqrtf(x)
#define ABS(x) fabsf(x)
#endif

// Store the options.
struct Options {
	feat scaleMin;
	feat scaleMax;
	feat scaleStep;
	feat binaryThre;
	int slideStep;
	int evidence;		// Number of neighboring detection to make sure.
	int modelHeight;
	int modelWidth;
	float invPerimeterRatio;	// Used in connected components analysis.
	float minAreaRatio;
	float maxAreaRatio;
};

#endif