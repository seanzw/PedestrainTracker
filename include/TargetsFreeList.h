/**
 * This is a container for muiltiple targets.
 *
 * @author Zhengrong Wang
 */

#ifndef TARGETS_FREELIST_HEADER
#define TARGETS_FREELIST_HEADER

#include "SingleTarget.h"

struct TargetsFreeListNode {
	SingleTarget *target;
	TargetsFreeListNode *nextFree;
	bool isFree;
	cv::Scalar color;

	TargetsFreeListNode(const Options &opts);
	~TargetsFreeListNode();
};

class TargetsFreeList {
public:
	TargetsFreeList(const Options &opts);
	~TargetsFreeList();

	/**
	 * Reset one target.
	 */
	void ResetOneTarget(int index);

	int InitializeTarget(const Rect &target, const Point2D &initVelocity);

	/**
	 * Propagate all the targets' particles.
	 */
	void Propagate(const Size &imgSize);

	/**
	 * Observe all the targets' partices.
	 */
	void Observe(const IntegralImage *intImage, const Pool<Rect> &detections);

	inline void Resample() {
		for (auto &node : listNodes) {
			if (!node.isFree)
				node.target->ResampleWithBest();
		}
	}

	/**
	 * Calculate the target-detection matching score matrix.
	 *
	 * @param intImage		integral image
	 * @param dets			detections
	 * @param matchMat		matching score matrix
	 */
	void CalculateMatchScore(const IntegralImage *intImage, 
		const Pool<Rect> &dets, std::vector<MatchMatrix::MatchScore> &matchMat) const;

	// The array of nodes.
	std::vector<TargetsFreeListNode> listNodes;
	const int capacity;

	// Weight for detection term in particle observation.
	const float detectionWeight;

	const std::vector<int> &GetMatchDets() { return matchDets; }

	/**
	 * Online training.
	 */
	void Train(const IntegralImage *intImage, const MultiSampler *multiSampler);

	inline void DrawTargets(cv::Mat &img, int index = -1) const {
		if (index < capacity && index >= 0 && !listNodes[index].isFree) {
			listNodes[index].target->DrawTarget(img, listNodes[index].color);
		}
		else {
			for (const auto &node : listNodes) {
				if (!node.isFree)
					node.target->DrawTarget(img, node.color);
			}
		}
	}

	inline void DrawParticles(cv::Mat &img, int index = -1) const {
		if (index < capacity && index >= 0 && !listNodes[index].isFree) {
			listNodes[index].target->DrawParticles(img, listNodes[index].color);
		}
		else {
			for (const auto &node : listNodes) {
				if (!node.isFree)
					node.target->DrawParticles(img, node.color);
			}
		}
	}

	inline void DrawMatches(cv::Mat &img, Pool<Rect> dets) {
		for (int i = 0; i < capacity; i++) {
			if (!listNodes[i].isFree && matchDets[i] != -1) {
				// We have a matched detection.
				// Draw the detection.
				cv::rectangle(img, (cv::Rect)dets[matchDets[i]], listNodes[i].color, 2);
			}
		}
	}

	// The (detection, target) pair.
	// Set by MatchMatrix.
	std::vector<int> matchDets;

private:

	TargetsFreeListNode *freeNodes;
};


#endif
