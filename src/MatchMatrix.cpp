#include "MatchMatrix.h"

MatchMatrix::MatchMatrix(int capacity) {
	matchMat = Pool<Pool<float>>(capacity);
}

MatchMatrix::~MatchMatrix() {

}

void MatchMatrix::SetNumDets(int numDets) {
	for (int i = 0; i < matchMat.size; i++) {
		//matchMat.
	}
}

void MatchMatrix::SetTargets(TargetsFreeList &targets, float threshold) {

}