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
	 *
	 * @param detection	The associate detection, if any.
	 */
	void Observe(const IntegralImage *intImage, const Rect *detection = NULL);

	/**
	 * Update the classifier.
	 * Only used when there are no overlapping detection
	 * and we are damn sure this is the correct one.
	 */
	void Update(const IntegralImage *intImage, const Rect &roi, int target, float importance = 1.0f);

	inline void ResampleWithBest() {
		particleFilter->ResampleWithBest();
	}

	inline void ResampleWithConfidence() {
		particleFilter->ResampleWithConfidence();
	}

private:

	StrongClassifierDirectSelect *classifier;
	ParticleFilterConstVelocity *particleFilter;

};

#endif