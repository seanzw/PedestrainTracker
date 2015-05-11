/**
 * This is a container for muiltiple targets.
 *
 * @author Zhengrong Wang
 */

#ifndef TARGETS_FREELIST_HEADER
#define TARGETS_FREELIST_HEADER

#include "SingleTarget.h"

struct TargetFreeListNode {
	SingleTarget *target;
	TargetFreeListNode *nextFree;
	bool isFree;

	TargetFreeListNode(int numParticles, 
		int numSelectors, int numWeakClassifiers, int numBackups);
	~TargetFreeListNode();
};

class TargetFreeList {
public:
	TargetFreeList(int capacity, int numParticles,
		int numSelectors, int numWeakClassifiers, int numBackups);
	~TargetFreeList();

	/**
	 * Reset one target.
	 */
	void ResetOneTarget(int index);

	int InitializeTarget(const Rect &target, const Point2D &initVelocity);

	void Propagate(const Size &imgSize);

	// void Update(int index, const IntegralImage *intImage, );

	// The array of nodes.
	std::vector<TargetFreeListNode> listNodes;
	const int capacity;

private:

	TargetFreeListNode *freeNodes;

};


#endif
