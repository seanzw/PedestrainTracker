#include "Feature.h"

Feature::Feature(int size) {
	capacity = size;
	if (size <= 0) {
		data = NULL;
	}
	else {
		data = new feat[capacity];
		memset((void *)data, 0, sizeof(feat) * capacity);
	}
}

Feature::~Feature() {
	if (data != NULL) {
		delete[] data;
	}
}

void Feature::Resize(int newSize) {
	if (newSize > capacity) {
		delete[] data;
		capacity = newSize;
		data = new feat[newSize];
		memset((void *)data, 0, sizeof(feat) * capacity);
	}
}