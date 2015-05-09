#include "TargetsFreeList.h"

TargetFreeListNode::TargetFreeListNode(int numParticles,
	int numSelectors, int numWeakClassifiers, int numBackups)
	: nextFree(NULL), isFree(true) {
	target = new SingleTarget(numParticles, Rect(), Point2D(),
		numSelectors, numWeakClassifiers, numBackups);
}

TargetFreeListNode::~TargetFreeListNode() {
	delete target;
}

TargetFreeList::TargetFreeList(int c, int numParticles,
	int numSelectors, int numWeakClassifiers, int numBackups) : capacity(c), freeNodes(NULL) {
	
	for (int i = 0; i < capacity; i++) {
		listNodes.push_back(TargetFreeListNode(numParticles, numSelectors,
			numWeakClassifiers, numBackups));
		listNodes[i].nextFree = freeNodes;
		freeNodes = &listNodes[i];
	}
}

TargetFreeList::~TargetFreeList() {

}

void TargetFreeList::ResetOneTarget(int index) {

	// This target is already free.
	if (listNodes[index].isFree) {
		throw 1;
	}

	listNodes[index].isFree = true;
	listNodes[index].target->ResetTarget();
	listNodes[index].nextFree = freeNodes;
	freeNodes = &listNodes[index];
}

int TargetFreeList::InitializeTarget(const Rect &target, const Point2D &initVelocity) {

	// We are out of capacity.
	if (freeNodes == NULL)
		return -1;

	TargetFreeListNode *newTarget = freeNodes;
	freeNodes = newTarget->nextFree;

	newTarget->target->InitializeTarget(target, initVelocity);
	newTarget->nextFree = NULL;
	newTarget->isFree = false;

	return (newTarget - &listNodes[0]) / sizeof(TargetFreeListNode);

}



