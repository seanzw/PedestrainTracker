#include "Geometry.h"

Size::Size(int w, int h) : width(w), height(h) {
}

Size Size::operator*(float f) {
	return Size((int)width * f, (int)height * f);
}

Size Size::operator=(const Rect &r) {
	height = r.height;
	width = r.width;
	return *this;
}

bool Size::operator==(const Size &other) const {
	return width == other.width && height == other.height;
}

bool Size::IsIn(const Rect &rect) const {
	if (rect.upper < 0 || rect.upper + rect.height > height)
		return false;
	if (rect.left < 0 || rect.left + rect.width > width)
		return false;
	return true;
}

bool Size::IsIn(int upper, int left, int width, int height) const {
	if (upper < 0 || upper + height > this->height)
		return false;
	if (left < 0 || left + width > this->width)
		return false;
	return true;
}

Rect Rect::operator+(const Point2D &offset) const {
	Rect ret = *this;
	ret.upper += offset.row;
	ret.left += offset.col;
	return ret;
}

Rect::operator cv::Rect() const {
	return cv::Rect(left, upper, width, height);
}

Rect::operator Size() const {
	return Size(width, height);
}

bool Rect::IsOverlap(const Rect &other) const {
	if (other.upper > upper + height || other.upper < upper - height)
		return false;
	if (other.left < left - width || other.left > left + width)
		return false;
	return true;
}


Point2D::Point2D(int r, int c) : row(r), col(c) {}

Point2D &Point2D::operator=(const Rect &r) {
	row = r.upper;
	col = r.left;
	return *this;
}