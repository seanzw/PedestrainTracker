/**
 * A single target used in multiple tracker.
 * It's very similar to ParticleFilterTracker.
 * It includes a target specific strong classifier, a particle filter.
 *
 * @author Zhengrong Wang.
 */

#ifndef SINGLE_TARGET_HEADER
#define SINGLE_TARGET_HEADER

#define ST_DEBUG

#include "Options.h"
#include "StrongClassifierDirectSelect.h"
#include "ParticleFilterConstVelocity.h"
#include "MultiSampler.h"
#include "SingleSampler.h"

#define SINGLE_TARGET_VELOCITY_CONST 10.0f

/**
 * Parameters for an empty target.
 *
 * @param nParticles			# particles
 * @param target				the target region
 * @param initVelocity			the initial velocity
 * @param numSelectors			# selectors in strong classifier
 * @param numWeakClassifiers	# weak classifiers
 * @param numBackups			# backup weak classifiers
 * @param distWeight			distance weight for match score
 * @param velocityThre			velocity threshold for match score
 * @param velocitySigmaConst	const number for velocity sigma
 */

class SingleTarget {
public:

	/**
	 * Construct an empty target.
	 */
	SingleTarget(const Options &ops);
	~SingleTarget();

	void InitializeTarget(const Rect &target, const Point2D &initVelocity);

	/**
	 * Propagate the particles.
	 * It sets the sigma for velocity and then call ParticleFilter::Propagate.
	 */
	void Propagate(const Size &imgSize);

	/**
	 * Train the classifier.
	 * Only used when there are no overlapping detection
	 * and we are damn sure this is the correct one.
	 * @param intImage		integral image
	 * @param multiSampler	where we can find the samples
	 * @param id			the id of this target
	 */
	void Train(const IntegralImage *intImage, const MultiSampler *multiSampler, int id);

    /**
     * Train with a single sampler.
     */
    inline void Train(const IntegralImage *intImage, const SingleSampler *sampler) {
        for (int i = 0; i < sampler->GetNumPos(); i++) {
            classifier->Update(intImage, sampler->GetPosSample(i), 1, 1.0f);
        }
        for (int i = 0; i < sampler->GetNumNeg(); i++) {
            classifier->Update(intImage, sampler->GetPosSample(i), -1, 1.0f);
        }
    }

	/**
	* Calculate the match score with all the detections.
	*
	* @param intImage		in: integral image
	* @param dets			in: detections
	* @param marchArray	out: the match score
	*/
	void CalculateMatchScore(const IntegralImage *intImage,
		const Pool<Rect> &dets,
		std::vector<MatchMatrix::MatchScore>::iterator &matchArray) const;

	/**
	 * Check if the det is near the target
	 */
	bool CheckNearbyTarget(const Rect &det, int distThre) const;

	/**
	 * Observe the particles.
	 * weight_particle = detectionWeight * P(particle, detection) + (1.0f - detectionWeight) * Conf(particle)
	 *
	 * @param detection			The associate detection, if any.
	 * @param detectionWeight	weight for detection term
	 * @param classifierWeight	weight for classifier term
	 */
	inline void Observe(const IntegralImage *intImage, const Rect &detection,
		float detectionWeight) {
		particleFilter->Observe(classifier, intImage, detection, detectionWeight);
	}

	/**
	 * Observe the particles without matched detections.
	 */
	inline void Observe(const IntegralImage *intImage) {
		particleFilter->Observe(classifier, intImage);
	}

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

	inline void DrawTarget(cv::Mat &img, const cv::Scalar &color) const {
		particleFilter->DrawTarget(img, color);
	}

	inline void DrawParticles(cv::Mat &img, const cv::Scalar &color) const {
		particleFilter->DrawParticles(img, color);
	}

	inline void DrawParticlesWithConfidence(cv::Mat &img, const cv::Scalar &color) const {
		particleFilter->DrawParticlesWithConfidence(img, color);
	}

    inline const Rect &GetTarget() const {
        return particleFilter->GetTarget();
    }

    inline void SetTarget(const Rect &t) {
        particleFilter->SetTarget(t);
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

	const float velocitySigmaConst;
};

#endif