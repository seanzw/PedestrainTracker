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
	ParticleFilter(StrongClassifier *classifier, IntegralImage *intImage, 
		const Rect &target, int n = 100);

	virtual ~ParticleFilter();

	// Core of particle filter.
	virtual void Propagate();
	virtual void Observe();

	// Draw the particles.
	void DrawParticles(cv::Mat &img, const cv::Scalar &color) const;

	// Draw the target;
	void DrawTarget(cv::Mat &img, const cv::Scalar &color) const;

protected:

	// The sizeof particles. Here it should be 2. Its structure like:
	// struct Particle {
	//		int upper;
	//		int left;
	// }
	const int numParticles;
	int *particles;
	static const int sizeParticle;
	
	float *confidence;

	Rect target;

	// Gaussian random generator.
	std::normal_distribution<float> gaussian;

	// Uniform random generator.
	std::uniform_real_distribution<float> resampler;

	// A buffer used in resampling.
	int *resampleBuffer;

	// Seed.
	static std::default_random_engine generator;

	// The online boosting classifier.
	StrongClassifier *classifier;

	// Feature extractor.
	IntegralImage *intImage;

	// Initialize the particles.
	void InitParticles();

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