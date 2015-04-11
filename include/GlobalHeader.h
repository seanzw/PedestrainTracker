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
#include "opencv\cv.h"

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

#endif