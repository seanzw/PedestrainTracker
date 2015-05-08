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

	TargetFreeListNode();
	~TargetFreeListNode();
};

class TargetFreeList {
public:
	TargetFreeList(int capacity, IntegralImage *intImage);
	~TargetFreeList();

	/**
	 * Reset one target.
	 */
	void ResetOneTarget(int index);

	int InitializeTarget(IntegralImage *intImage);
	
	void Update(int index, const IntegralImage *intImage, )

private:

	// The array of nodes.
	TargetFreeListNode *listNodes;

};


#endif
