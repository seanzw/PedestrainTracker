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
	ParticleFilterConstVelocity(int n);

	~ParticleFilterConstVelocity();

	void InitBuffer();
	void InitTarget(const Rect &target, const Point2D &initVelocity);

	void Propagate(const Size &imgSize);

	void SetVelocitySigma(float sigma);

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

	void InitParticles();
};

#endif