/**
 * @author Zhengrong Wang.
 */
#ifndef GEOMETRY_HEADER
#define GEOMETRY_HEADER

#define ONEOVERSQRT2PI 0.39894228f

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

	inline explicit operator cv::Size() const {
		return cv::Size(width, height);
	}

	bool IsIn(const Rect &rect) const;
	bool IsIn(int upper, int left, int width, int height) const;
};

class Rect {
public:
	Rect(int u = 0, int l = 0, int w = 0, int h = 0)
		: upper(u), left(l), width(w), height(h) {}

	int upper;
	int left;
	int height; 
	int width;

	// Shift this rectangle by an offset.
	Rect operator+(const Point2D &offset) const;

	// Explicit conversion.
	explicit operator cv::Rect() const;

	explicit operator Size() const;
};

class Point2D {
public:
	Point2D(int r = 0, int c = 0);

	Point2D &operator=(const Rect &r);

	inline float SquaredDistance(const Point2D &other) const {
		return (row - other.row) * (row - other.row) + (col - other.col) * (col - other.col);
	}

	inline float Distance(const Point2D &other) const {
		return sqrtf(SquaredDistance(other));
	}

	int row, col;
};

inline float GetGaussianProb(float mean, float sigma, float value) {
	float invSimga = 1.0f / sigma;
	return ONEOVERSQRT2PI * invSimga * expf(-0.5 * powf((value - mean) * invSimga, 2.0));
}

#endif