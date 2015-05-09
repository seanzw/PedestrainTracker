/**
 * A basic particle filter.
 * It just propagates the particles and observe using
 * the strong classifier's evaluate function. We leave
 * the training to upper class.
 * @author Zhengrong Wang.
 */

#ifndef PARTICLEFILTER_HEADER
#define PARTICLEFILTER_HEADER

#include "StrongClassifier.h"
#include "IntegralImage.h"

class ParticleFilter {
public:

	/**
	 * This constructor doesn't initialize particles.
	 * Used for child class such as ParticleFitlerConstVelocity.
	 */
	ParticleFilter(int n = 100, int szParticle = 2);

	ParticleFilter(const Rect &target, int n = 100, int szParticle = 2);

	virtual ~ParticleFilter();

	/**
	 * Propagate the particles.
	 * 
	 * @param imgSize	Is the particle still inside the image?
	 */
	virtual void Propagate(const Size &imgSize);

	/**
	 * Observe with classifier output confidence.
	 */
	virtual void Observe(StrongClassifier *classifier, const IntegralImage *intImage);

	/**
	 * Observe with classifier output confidence and associated detection.
	 */
	virtual void Observe(StrongClassifier *classifier, const IntegralImage *intImage,
		const Rect &detection, float detectionWeight, float classifierWeight);

	/**
	 * Resample directly around the best particle.
	 */
	void ResampleWithBest();
	
	/**
	 * Resample with the confidence.
	 */
	void ResampleWithConfidence();


	// Draw the particles.
	void DrawParticles(cv::Mat &img, const cv::Scalar &color) const;
	void DrawParticlesWithConfidence(cv::Mat &img, const cv::Scalar &color) const;

	// Draw the target;
	void DrawTarget(cv::Mat &img, const cv::Scalar &color) const;

	const Rect &GetTarget() const;

	// Initialize the particles.
	virtual void InitParticles();

protected:

	// The sizeof particles. Here it should be 2. Its structure like:
	// struct Particle {
	//		int upper;
	//		int left;
	// }
	const int numParticles;
	int *particles;
	const int sizeParticle;
	
	float *confidence;

	Rect target;

	// Seed.
	std::default_random_engine generator;

	// Gaussian random generator.
	std::normal_distribution<float> gaussian;

	// Uniform random generator.
	std::uniform_real_distribution<float> resampler;

	// A buffer used in resampling.
	int *resampleBuffer;

	// Binary search, used in resample.
	inline int BinarSearch(float prob) const {
		// A corner case.
		if (prob <= confidence[0])
			return 0;
		int start = 0;
		int end = numParticles - 1;
		int mid = (start + end) / 2;
		while (start < end - 1) {
			if (prob <= confidence[mid]) {
				end = mid;
			}
			else {
				start = mid;
			}
			mid = (start + end) / 2;
		}
		return end;
	}

};

#endif