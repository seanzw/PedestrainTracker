#include "ParticleFilterConstVelocity.h"

const int ParticleFilter::sizeParticle = 4;

std::default_random_engine ParticleFilter::generator;

ParticleFilterConstVelocity::ParticleFilterConstVelocity(StrongClassifier *c, IntegralImage *i,
	const Rect &t, const Point2D &v, int n) : ParticleFilter(n) {

	classifier = c;
	intImage = i;
	target = t;

	particles = new int[numParticles * sizeParticle];
	InitParticles(v);
	confidence = new float[numParticles];
	resampleBuffer = new int[numParticles * sizeParticle];

}

void ParticleFilterConstVelocity::InitParticles(const Point2D &v) {

}

ParticleFilterConstVelocity::~ParticleFilterConstVelocity() {

}

void ParticleFilterConstVelocity::Propagate(const Size &imgSize) {

}