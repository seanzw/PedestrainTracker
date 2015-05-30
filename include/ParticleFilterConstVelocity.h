/**
 * ParticleFilter with constant velocity.
 * 
 * @author Zhengrong Wang.
 */

#ifndef PARTICLEFILTER_CONSTVELOCITY_HEADER
#define PARTICLEFILTER_CONSTVELOCITY_HEADER

#include "ParticleFilter.h"
#include "Pool.h"

class ParticleFilterConstVelocity : public ParticleFilter {
public:
	ParticleFilterConstVelocity(int n, float velocityThre, float distWeight);

	~ParticleFilterConstVelocity();

	void InitBuffer();
	void InitTarget(const Rect &target, const Point2D &initVelocity);

	void Propagate(const Size &imgSize);

	void CalculateMatchScore(const IntegralImage *intImage,
		const StrongClassifier *classifier, const Pool<Rect> &dets,
		std::vector<MatchMatrix::MatchScore>::iterator &matchArray) const;

	//inline void SetVelocity(const Point2D &initV) {
	//	velocity = initV;
	//}
	void SetVelocitySigma(float sigma);

protected:
	// The sizeof particles. Here it should be 2. Its structure like:
	// struct Particle {
	//		int upper;
	//		int left;
	//		int vUpper;
	//		int vLeft;
	// }

	Point2D velocity;

	// The threshold of velocity used in gate function.
	const float velocityThre;

	// The weight of distance score in match score formula.
	const float distWeight;

	// Gaussian noise used in velocity.
	std::normal_distribution<float> gaussianVelocity;

	void InitParticles();
};

#endif