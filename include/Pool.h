/*************************************************************
 Pool template.
 Use std::vector as the container, however it never shrinks.
 Author: Zhengrong Wang.
 *************************************************************/

#ifndef POOL_HEADER
#define POOL_HEADER

#include "GlobalHeader.h"

template<typename T> class Pool {
public:
	// The size of the pool.
	int size;
	// Data container.
	std::vector<T> data;

	// Constructor.
	Pool() : size(0) {}

	// Overload the operator.
	T &operator[](uint index) {
		return data[index];
	}

	void Push(const T &t) {
		if (size == data.size())
			data.push_back(t);
		else
			data[size] = t;
		size++;
	}

	// Reset the pool;
	void clear() {
		size = 0;
	}
};

#endif