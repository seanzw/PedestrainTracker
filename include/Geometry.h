/********************************************
 Geometry file.
 Define some useful geometric class.
 Author: Zhengrong Wang.
 ********************************************/
#ifndef GEOMETRY_HEADER
#define GEOMETRY_HEADER

#include "GlobalHeader.h"

class Rect;
class Point2D;

class Size {
public:

	Size(int w = 0, int h = 0);
	
	int width, height;
	int area;

	Size operator=(const Rect &r);
	Size operator*(float f);
	bool operator==(const Size &other) const;
};

class Rect {
public:
	Rect(int u = 0, int l = 0, int w = 0, int h = 0)
		: upper(u), left(l), width(w), height(h), area(height * width) {}

	int upper;
	int left;
	int height; 
	int width;
	int area;

	// Shift this rectangle by an offset.
	Rect operator+(const Point2D &offset) const;

	// Explicit conversion.
	explicit operator cv::Rect() const;
};

class Point2D {
public:
	Point2D(int r = 0, int c = 0);

	Point2D &operator=(const Rect &r);

	int row, col;
};

#endif