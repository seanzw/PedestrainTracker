/**
 * Use std::vector as the container, however it never shrinks.
 * @author Zhengrong Wang.
 */

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
	Pool(int sz = 0) : size(sz) { data = std::vector<T>(sz); }

	// Overload the operator.
	T &operator[](uint index) {
		return data[index];
	}

	const T &operator[](uint index) const {
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

	void Resize(int sz) {
		if (sz > size) {
			data.resize(sz);
		}
		size = sz;
	}
};

#endif