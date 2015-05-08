/**
 * ParticleFilter with constant velocity.
 * 
 * @author Zhengrong Wang.
 */

#ifndef PARTICLEFILTER_CONSTVELOCITY_HEADER
#define PARTICLEFILTER_CONSTVELOCITY_HEADER

#include "ParticleFilter.h"

class ParticleFilterConstVelocity : public ParticleFilter {
public:
	ParticleFilterConstVelocity(const Rect &target, const Point2D &initVelocity = Point2D(10, 0), int n = 100);

	virtual ~ParticleFilterConstVelocity();

	virtual void Propagate(const Size &imgSize);

	void SetVelocitySigma(float sigma);

	virtual void InitParticles();

protected:
	// The sizeof particles. Here it should be 2. Its structure like:
	// struct Particle {
	//		int upper;
	//		int left;
	//		int vUpper;
	//		int vLeft;
	// }

	Point2D initVelocity;

	// Gaussian noise used in velocity.
	std::normal_distribution<float> gaussianVelocity;
};

#endif