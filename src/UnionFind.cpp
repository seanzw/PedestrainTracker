#include "UnionFind.h"

UnionFind::UnionFind(int N) {
	num = N;
	count = N;
	parent = new int[N];
	size = new int[N];

	for (int i = 0; i < N; i++) {
		parent[i] = i;
		size[i] = 1;
	}
}

UnionFind::~UnionFind() {
	delete[] parent;
	delete[] size;
}

int UnionFind::Find(int p) const {
	if (!Validate(p)) {
		return -1;
	}
	while (p != parent[p]) {
		
		// Amazing one line path compression.
		parent[p] = parent[parent[p]];

		p = parent[p];
	}
	return p;
}

void UnionFind::Union(int p, int q) {
	int rootP = Find(p);
	int rootQ = Find(q);

	if (rootP == rootQ)
		return;

	// make smaller root pointer to larger one.
	if (size[rootP] < size[rootQ]) {
		parent[rootP] = rootQ;
		size[rootQ] += size[rootP];
	}
	else {
		parent[rootQ] = rootP;
		size[rootP] += size[rootQ];
	}
	count--;
}

bool UnionFind::Validate(int p) const {
	return p >= 0 && p < num;
}