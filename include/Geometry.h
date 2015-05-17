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
		return (float)((row - other.row) * (row - other.row) + (col - other.col) * (col - other.col));
	}

	inline float Distance(const Point2D &other) const {
		return sqrtf(SquaredDistance(other));
	}

	inline float SquaredLength() const {
		return (float)(row * row + col * col);
	}

	inline float Length() const {
		return sqrtf(SquaredLength());
	}

	inline Point2D operator-(const Point2D &other) const {
		return Point2D(row - other.row, col - other.col);
	}

	inline float operator*(const Point2D &other) const {
		return (float)(row * other.row + col * other.col);
	}

	inline Point2D operator*(float scale) const {
		return Point2D(row * scale, col * scale);
	}

	/**
	 * Given two vector this and other,
	 * returns |other - dot(this, other) / (|this| ^ 2) * this|.
	 */
	inline float ProjectResidual(const Point2D &other) const {
		float dot = (*this) * other;
		return (other - (*this)*(dot / SquaredLength())).Length();
	}

	int row, col;
};

inline float GetGaussianProb(float mean, float sigma, float value) {
	float invSimga = 1.0f / sigma;
	return ONEOVERSQRT2PI * invSimga * expf(-0.5f * powf((value - mean) * invSimga, 2.0f));
}

#endif