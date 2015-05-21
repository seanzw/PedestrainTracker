#include "TargetsFreeList.h"

TargetsFreeListNode::TargetsFreeListNode(const Options &opts)
	: nextFree(NULL), isFree(true) {

	target = new SingleTarget(opts);
}

TargetsFreeListNode::~TargetsFreeListNode() {
	delete target;
}

TargetsFreeList::TargetsFreeList(const Options &opts)
	: capacity(opts.targetsFreeListCapacity), freeNodes(NULL), detectionWeight(opts.detectionWeight) {

	for (int i = 0; i < capacity; i++) {
		listNodes.push_back(TargetsFreeListNode(opts));
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

void TargetsFreeList::Propagate(const Size &imgSize) {
	for (int i = 0; i < capacity; i++) {
		if (!listNodes[i].isFree) {
			listNodes[i].target->Propagate(imgSize);
		}
	}
}

void TargetsFreeList::Observe(const IntegralImage *intImage, const Pool<Rect> &detections) {
	for (int i = 0; i < capacity; i++) {
		if (!listNodes[i].isFree) {
			// Look up for a mach detection.
			if (matchDets[i] != -1) {
				// This target has a matched detection.
				// Update the detection sequence.
				listNodes[i].target->UpdateSeq(true);
				
				// Observe.
				listNodes[i].target->Observe(intImage, detections[matchDets[i]], detectionWeight);
			}
			else {
				// This target has no matched detection.
				// Update the detection sequence.
				listNodes[i].target->UpdateSeq(false);

				// Observe.
				listNodes[i].target->Observe(intImage);
			}
		}
	}
}

void TargetsFreeList::Train(const IntegralImage *intImage, const MultiSampler *multiSampler) {
	for (int i = 0; i < capacity; i++) {
		if (!listNodes[i].isFree && matchDets[i] != -1) {
			// We have matched detection.
			listNodes[i].target->Train(intImage, multiSampler, i);
		}
	}
}