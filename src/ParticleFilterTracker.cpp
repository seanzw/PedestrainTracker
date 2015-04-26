#include "ParticleFilterTracker.h"

ParticleFilterTracker::ParticleFilterTracker(StrongClassifier *c, IntegralImage *i, ParticleFilter *p)
	: classifier(c), intImage(i), particleFilter(p) {

}

ParticleFilterTracker::~ParticleFilterTracker() {

}

void ParticleFilterTracker::Track(cv::VideoCapture &in, cv::VideoWriter &out) {

}