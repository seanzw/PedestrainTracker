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

Rect Rect::operator+(const Point2D &offset) const {
	Rect ret = *this;
	ret.upper += offset.row;
	ret.left += offset.col;
	return ret;
}


Point2D::Point2D(int r, int c) : row(r), col(c) {}

Point2D &Point2D::operator=(const Rect &r) {
	row = r.upper;
	col = r.left;
	return *this;
}