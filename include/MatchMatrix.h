/**
 * Use greedy algorithm to find best (detection, target) pair.
 */

#ifndef MATCH_MATRIX_HEADER
#define MATCH_MATRIX_HEADER

#include "Pool.h"

class MatchMatrix {
public:
	MatchMatrix(int capacity);
	~MatchMatrix();

	struct MatchScore {
		float score;
		int target;
		int detection;

		MatchScore() : target(-1), detection(-1), score(0.0f) {}

		bool operator<(const MatchScore &other) const {
			return score < other.score;
		}

		/**
		 * Used to sort in descending order.
		 */
		bool operator>(const MatchScore &other) const {
			return score > other.score;
		}
	};

	/**
	 * Set the number of detections.
	 * Should be called before calculating match score matrix.
	 */
	void SetNumDets(int numDets);

	/**
	 * After calculating the matching score matrix, use greedy algorithm to find
	 * the (target, detection) pair. Only match score > threshold are considered.
	 */
	void SetTargets(std::vector<int> &targets, float threshold);

	/**
	 * Print the match score matrix for debugging.
	 */
	void PrintMatchMatrix();

	/**
	 * How many targets?
	 */
	const int capacity;

	/**
	 * The match score matrix.
	 * It should be (capacity \times # detections).
	 */
	std::vector<MatchScore> matchMatrix;

	/**
	 * Is this detection has already matched to some targert?
	 * Avoid using std::vector<char>.
	 */
	std::vector<char> isDetMatched;
};


#endif
