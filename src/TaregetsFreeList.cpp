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

	matchDets = std::vector<int>(capacity);
	listNodes.reserve(capacity);

	for (int i = 0; i < capacity; i++) {
		listNodes.emplace_back(opts);
		listNodes[i].color = cv::Scalar(rand() % 256, rand() % 256, rand() % 256);
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

    // Randomly choose a new color.
    newTarget->color = cv::Scalar(rand() % 256, rand() % 256, rand() % 256);

	// Get the index of the new target.
	return newTarget - &listNodes[0];

}

void TargetsFreeList::CalculateMatchScore(const IntegralImage *intImage,
	const Pool<Rect> &dets, std::vector<MatchMatrix::MatchScore> &matchMat) const {
	for (int i = 0; i < capacity; i++) {
		if (!listNodes[i].isFree) {
			auto iter = matchMat.begin() + i * dets.size;
			listNodes[i].target->CalculateMatchScore(intImage, dets, iter);
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
                // Set the size the same as the detection.
                // listNodes[i].target->SetTarget(detections[matchDets[i]]);

				// Update the detection sequence.
				 listNodes[i].target->UpdateSeq(true);
				
				// Observe.
				listNodes[i].target->Observe(intImage, detections[matchDets[i]], detectionWeight);
			}
			else {
				// This target has no matched detection.
				// Update the detection sequence.
				listNodes[i].target->UpdateSeq(true);

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

void TargetsFreeList::Train(const IntegralImage *intImage, SingleSampler *sampler, const Size imgSize) {
    for (int i = 0; i < capacity; i++) {
        if (!listNodes[i].isFree) {
            // We have matched detection.
            sampler->Sample(listNodes[i].target->GetTarget(), imgSize);
            listNodes[i].target->Train(intImage, sampler);
        }
    }
}

void TargetsFreeList::Train(int idx,
    const IntegralImage *intImage, const SingleSampler *sampler) {
    listNodes[idx].target->Train(intImage, sampler);
}



bool TargetsFreeList::CheckNearbyTarget(const Rect &det, int distThre) const {
    int index = 0;
	for (const auto &node : listNodes) {
		if (!node.isFree && matchDets[index] == -1) {
			if (node.target->CheckNearbyTarget(det, distThre)) {
				return true;
			}
		}
        index++;
	}
	return false;
}