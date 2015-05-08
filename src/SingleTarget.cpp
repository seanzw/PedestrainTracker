#include "SingleTarget.h"

SingleTarget::SingleTarget(int nParticles, const Rect &target, const Point2D &velocity,
	int numSelectors, int numWeakClassifiers, int numBackups) {

	// Initialize the strong classifier.
	classifier = new StrongClassifierDirectSelect(numSelectors, numWeakClassifiers,
		(Size)target, numBackups);

	// Initialize the particle filter.
	particleFilter = new ParticleFilterConstVelocity(target, velocity, nParticles);

}

SingleTarget::~SingleTarget() {
	delete classifier;
	delete particleFilter;
}

void SingleTarget::ResetTarget() {
	// TO DO
}

void SingleTarget::InitializeTarget(const Rect &target, const Point2D &initVelocity) {
	// TO DO
}

void SingleTarget::Propagate(const Size &imgSize) {
	
	// First we need to set the sigma for velocity
	// according to how many frames we have successfully detected.
	// TO DO
	// particleFilter->SetVelocitySigma();

	// Propagate the particles.
	particleFilter->Propagate(imgSize);
}

void SingleTarget::Observe(const IntegralImage *intImage, const Rect *detection) {
	// TO DO

}

void SingleTarget::Update(const IntegralImage *intImage, const Rect &roi, int target, float importance) {
	// TO DO


}




