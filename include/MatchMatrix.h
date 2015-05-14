/**
 * Use greedy algorithm to find best (detection, target) pair.
 */

#ifndef MATCH_MATRIX_HEADER
#define MATCH_MATRIX_HEADER

#include "Pool.h"
#include "TargetsFreeList.h"

class MatchMatrix {
public:
	MatchMatrix(int capacity);
	~MatchMatrix();

	/**
	 * Set the number of detections.
	 * Should be called before calculating match score matrix.
	 */
	void SetNumDets(int numDets);

	/**
	 * After calculating the matching score matrix, use greedy algorithm to find
	 * the (target, detection) pair. Only match score > threshold are considered.
	 */
	void SetTargets(TargetsFreeList &targets, float threshold);

	/**
	 * The match score matrix.
	 * It should be (capacity \times # detections).
	 */
	Pool<Pool<float>> matchMat;
};


#endif
