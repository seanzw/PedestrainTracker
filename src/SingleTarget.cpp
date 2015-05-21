#include "SingleTarget.h"

SingleTarget::SingleTarget(const Options &opts)
	: detectionSeq(0), velocitySigmaConst(opts.velocitySigmaConst) {

	// Initialize the strong classifier.
	classifier = new StrongClassifierDirectSelect(opts.numSelectors,
		opts.numWeakClassifiers,
		(Size)opts.target,
		opts.numBackups);

	// Construct the particle filter.
	particleFilter = new ParticleFilterConstVelocity(opts.nParticles,
		opts.velocityThre,
		opts.distWeight);
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

void SingleTarget::Train(const IntegralImage *intImage, const MultiSampler *multiSampler, int id) {

	// Update the classifier.
	for (MultiSampler::const_iterator iter = multiSampler->begin(id);
		iter != multiSampler->end();
		++iter) {
		classifier->Update(intImage, *iter, iter.GetTarget(), 1.0f);
	}
}

void SingleTarget::CalculateMatchScore(const IntegralImage *intImage,
	const Pool<Rect> &dets, Pool<float> &matchArray) const {

	particleFilter->CalculateMatchScore(intImage, classifier, dets, matchArray);
}



