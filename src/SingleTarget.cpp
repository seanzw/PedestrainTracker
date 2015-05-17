#include "SingleTarget.h"

SingleTarget::SingleTarget(int nParticles, const Rect &target, const Point2D &velocity,
	int numSelectors, int numWeakClassifiers, int numBackups) : detectionSeq(0) {

	// Initialize the strong classifier.
	classifier = new StrongClassifierDirectSelect(numSelectors, numWeakClassifiers,
		(Size)target, numBackups);

	// Construct the particle filter.
	particleFilter = new ParticleFilterConstVelocity(nParticles);
	particleFilter->InitBuffer();
}

SingleTarget::~SingleTarget() {
	delete classifier;
	delete particleFilter;
}

void SingleTarget::InitializeTarget(const Rect &target, const Point2D &initVelocity) {
	
	// Initialize the strong classifier.
	classifier->Initialize((Size)target);

	// Initialize the particle filter.
	particleFilter->InitTarget(target, initVelocity);

	// Reset the detection sequence.
	detectionSeq = 0;
}

void SingleTarget::Propagate(const Size &imgSize) {
	
	// First we need to set the sigma for velocity
	// according to how many frames we have successfully detected.
	float velocitySigma;
	if (detectionSeq == 0) {
		velocitySigma = SINGLE_TARGET_VELOCITY_CONST;
	}
	else {
		velocitySigma = SINGLE_TARGET_VELOCITY_CONST / (float)detectionSeq;
	}
	particleFilter->SetVelocitySigma(velocitySigma);

	// Propagate the particles.
	particleFilter->Propagate(imgSize);
}

void SingleTarget::Update(const IntegralImage *intImage, const Rect &roi, int target, float importance) {
	// TO DO
	classifier->Update(intImage, roi, target, importance);

}

void SingleTarget::CalculateMatchScore(const IntegralImage *intImage,
	const Pool<Rect> &dets, Pool<float> &matchArray) const {
	// TO DO
}


