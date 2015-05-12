/**
 * A single target used in multiple tracker.
 * It's very similar to ParticleFilterTracker.
 * It includes a target specific strong classifier, a particle filter.
 *
 * @author Zhengrong Wang.
 */

#ifndef SINGLE_TARGET_HEADER
#define SINGLE_TARGET_HEADER

#include "StrongClassifierDirectSelect.h"
#include "ParticleFilterConstVelocity.h"
#include "IntegralImage.h"

class SingleTarget {
public:

	/**
	 * Construct an empty target.
	 *
	 * @param nParticles			# particles
	 * @param target				the target region
	 * @param initVelocity			the initial velocity
	 * @param numSelectors			# selectors in strong classifier
	 * @param numWeakClassifiers	# weak classifiers
	 * @param numBackups			# backup weak classifiers
	 */
	SingleTarget(int nParticles, const Rect &target, const Point2D &initVelocity,
		int numSelectors, int numWeakClassifiers, int numBackups = 0);
	~SingleTarget();

	void InitializeTarget(const Rect &target, const Point2D &initVelocity);
	void ResetTarget();

	/**
	 * Propagate the particles.
	 * It sets the sigma for velocity and then call ParticleFilter::Propagate.
	 */
	void Propagate(const Size &imgSize);

	/**
	 * Observe the particles.
	 * weight_particle = detectionWeight * P(particle, detection) + classifierWeight * Conf(particle)
	 *
	 * @param detection			The associate detection, if any.
	 * @param detectionWeight	weight for detection term
	 * @param classifierWeight	weight for classifier term
	 */
	void Observe(const IntegralImage *intImage, const Rect *detection,
		float detectionWeight, float classifierWeight);

	/**
	 * Update the classifier.
	 * Only used when there are no overlapping detection
	 * and we are damn sure this is the correct one.
	 */
	void Update(const IntegralImage *intImage, const Rect &roi, int target, float importance = 1.0f);

	/**
	 * Update the detection sequence.
	 */
	inline void UpdateSeq(bool isDetected) {
		detectionSeq >>= 1;
		if (isDetected)
			detectionSeq |= 0x08;
	}

	inline void ResampleWithBest() {
		particleFilter->ResampleWithBest();
	}

	inline void ResampleWithConfidence() {
		particleFilter->ResampleWithConfidence();
	}

private:

	StrongClassifierDirectSelect *classifier;
	ParticleFilterConstVelocity *particleFilter;

	/**
	 * This variable records the detections in the last 4 frames.
	 * It is set to one if in that frame this target is detected.
	 * The current frame is MSB.
	 *
	 *				[current]  [current - 4]
	 *					|			|
	 *	0	0	0	0	0	0	0	0
	 *
	 * The sigma for velocity is set as:
	 * v_sigma = 1.0f / (float)detectionSeq
	 *
	 */
	uchar detectionSeq;
};

#endif