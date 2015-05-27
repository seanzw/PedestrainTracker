/**
 * This is the global header for pedestrain counting.
 * Define some global constants.
 */

#ifndef GLOBAL_HEADER
#define GLOBAL_HEADER

// Switch this to use rgi or gray haar feature.
#define USE_RGI_FEATURE
// #define USE_GRAY_FEATURE

#include <cstdio>
#include <math.h>
#include <fstream>
#include <iomanip>
#include <string.h>
#include <random>
#include <algorithm>
#include <functional>

#include "opencv\cv.h"
#include "opencv\highgui.h"
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

// This is used as the default background reference.
static cv::Mat defaultBackground;

#endif