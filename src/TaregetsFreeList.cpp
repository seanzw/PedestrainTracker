#include "TargetsFreeList.h"

TargetsFreeListNode::TargetsFreeListNode(int numParticles,
	int numSelectors, int numWeakClassifiers, int numBackups)
	: nextFree(NULL), isFree(true) {
	target = new SingleTarget(numParticles, Rect(), Point2D(),
		numSelectors, numWeakClassifiers, numBackups);
}

TargetsFreeListNode::~TargetsFreeListNode() {
	delete target;
}

TargetsFreeList::TargetsFreeList(int c, int numParticles,
	int numSelectors, int numWeakClassifiers, int numBackups) : capacity(c), freeNodes(NULL) {
	
	for (int i = 0; i < capacity; i++) {
		listNodes.push_back(TargetsFreeListNode(numParticles, numSelectors,
			numWeakClassifiers, numBackups));
		listNodes[i].nextFree = freeNodes;
		freeNodes = &listNodes[i];
	}
}

TargetsFreeList::~TargetsFreeList() {

}

void TargetsFreeList::ResetOneTarget(int index) {

	// This target is already free.
	if (listNodes[index].isFree) {
		throw 1;
	}

	listNodes[index].isFree = true;
	listNodes[index].target->ResetTarget();
	listNodes[index].nextFree = freeNodes;
	freeNodes = &listNodes[index];
}

int TargetsFreeList::InitializeTarget(const Rect &target, const Point2D &initVelocity) {

	// We are out of capacity.
	if (freeNodes == NULL)
		return -1;

	TargetsFreeListNode *newTarget = freeNodes;
	freeNodes = newTarget->nextFree;

	newTarget->target->InitializeTarget(target, initVelocity);
	newTarget->nextFree = NULL;
	newTarget->isFree = false;

	// Get the index of the new target.
	return (newTarget - &listNodes[0]) / sizeof(TargetsFreeListNode);

}

void TargetsFreeList::CalculateMatchScore(const IntegralImage *intImage,
	const Pool<Rect> &dets, Pool<Pool<float>> &matchMat) const {
	for (int i = 0; i < capacity; i++) {
		if (!listNodes[i].isFree) {
			listNodes[i].target->CalculateMatchScore(intImage, dets, matchMat[i]);
		}
	}
}



