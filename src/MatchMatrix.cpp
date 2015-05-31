#include "MatchMatrix.h"

MatchMatrix::MatchMatrix(int c) : capacity(c) {
	
}

MatchMatrix::~MatchMatrix() {

}

void MatchMatrix::SetNumDets(int numDets) {

	// Resize the match matrix.
	matchMatrix.resize(numDets * capacity);

	// Initialize every thing.
	int target = 0, detection = 0;
	for (auto &matchScore : matchMatrix) {
		matchScore.score = 0.0f;
		matchScore.target = target;
		matchScore.detection = detection;
		detection++;
		
		// Reset the target and detection.
		if (detection == numDets) {
			target++;
			detection = 0;
		}
	}

	// Initialize the bool vector.
	isDetMatched.resize(numDets);
	for (auto &is : isDetMatched) {
		is = 0;
	}
}

void MatchMatrix::SetTargets(std::vector<int> &targets, float threshold) {
	
	// First set everything to -1.
	for (auto &target : targets) {
		target = -1;
	}

	// Sort the match score in descending order.
	std::sort(matchMatrix.begin(), matchMatrix.end(), std::greater<MatchScore>());
	
	// Start to set the target-detection match.
	for (const auto &matchScore : matchMatrix) {
		if (matchScore.score < threshold) {
			// We are done.
			break;
		}

		// Check if this target or detection has been matched.
		if (targets[matchScore.target] != -1 || isDetMatched[matchScore.detection] != 0) {
			continue;
		}

		// We find a new match.
		targets[matchScore.target] = matchScore.detection;
		isDetMatched[matchScore.detection] = 1;
	}
}

void MatchMatrix::PrintMatchMatrix() {
	// The current row.
	int curRow = 0;
	printf("======= Start Print Match Score Matrix ======\n");
	for (const auto &matachScore : matchMatrix) {
		if (matachScore.target > curRow) {
			printf("\n");
			curRow++;
		}
		printf("  %f  ", matachScore.score);
	}
	printf("\n====== Finish Print Match Score Matrix ======\n");
}