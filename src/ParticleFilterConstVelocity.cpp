#include "ParticleFilterConstVelocity.h"

ParticleFilterConstVelocity::ParticleFilterConstVelocity(const Rect &t, const Point2D &v, int n) : ParticleFilter(n, 4) {

	target = t;
	initVelocity = v;

	particles = new int[numParticles * sizeParticle];
	InitParticles();
	confidence = new float[numParticles];
	resampleBuffer = new int[numParticles * sizeParticle];

}

void ParticleFilterConstVelocity::InitParticles() {
	int *curParticle = particles;
	int upper = target.upper;
	int left = target.left;

	// Initialize the gaussian distribution according to the size.
	gaussian = std::normal_distribution<float>(0.0f, target.width * 0.125f);

	for (int i = 0; i < numParticles; i++, curParticle += sizeParticle) {
		curParticle[0] = upper + (int)gaussian(generator);
		curParticle[1] = left + (int)gaussian(generator);
		curParticle[3] = initVelocity.row;
		curParticle[4] = initVelocity.col;
	}
}

ParticleFilterConstVelocity::~ParticleFilterConstVelocity() {
	// ~ParticleFilter should delete all stuff.
}

void ParticleFilterConstVelocity::Propagate(const Size &imgSize) {
	int *curParticle = particles;
	for (int i = 0; i < numParticles; i++, curParticle += sizeParticle) {
		while (true) {
			curParticle[0] += (curParticle[2] + (int)gaussian(generator));
			curParticle[1] += (curParticle[3] + (int)gaussian(generator));
			if (imgSize.IsIn(curParticle[0], curParticle[1], target.width, target.height)) {
				curParticle[2] += (int)gaussianVelocity(generator);
				curParticle[3] += (int)gaussianVelocity(generator);
				break;
			}
		}
	}
}

void ParticleFilterConstVelocity::SetVelocitySigma(float sigma) {
	gaussianVelocity = std::normal_distribution<float>(0.0f, sigma);
}