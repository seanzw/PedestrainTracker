#include "ParticleFilter.h"

const int ParticleFilter::sizeParticle = 2;

std::default_random_engine ParticleFilter::generator;

ParticleFilter::ParticleFilter(StrongClassifier *c, IntegralImage *i,
	const Rect &t, int n) : numParticles(n), classifier(c), intImage(i), target(t) {
	InitParticles();

	confidence = new float[numParticles];

	resampleBuffer = new int[numParticles * sizeParticle];
}

void ParticleFilter::InitParticles() {
	particles = new int[numParticles * sizeParticle];
	int *curPartice = particles;
	int upper = target.upper;
	int left = target.left;

	// Initialize the gaussian distribution according to the size.
	gaussian = std::normal_distribution<float>(0.0f, 5.0f);

	for (int i = 0; i < numParticles; i++, curPartice += sizeParticle) {
		curPartice[0] = upper + (int)gaussian(generator);
		curPartice[1] = left + (int)gaussian(generator);
	}
}

ParticleFilter::~ParticleFilter() {
	delete[] particles;
	delete[] resampleBuffer;
	delete[] confidence;
}

void ParticleFilter::Propagate() {
	int *curParticle = particles;
	for (int i = 0; i < numParticles; i++, curParticle += sizeParticle) {
		curParticle[0] += (int)gaussian(generator);
		curParticle[1] += (int)gaussian(generator);
	}
}

void ParticleFilter::Observe() {
	int *curParticle = particles;
	Rect roi(0, 0, target.width, target.height);

	// Minimum confidence... I think we can do it better...
	// Maybe normalize the confidence in the lower classifier level...
	float minimumConf = FLT_MAX;

	for (int i = 0; i < numParticles; i++, curParticle += sizeParticle) {
		// Set the roi.
		roi.upper = curParticle[0];
		roi.left = curParticle[1];

		// Evaluate the particle.
		confidence[i] = classifier->Evaluate(intImage, roi);
		if (confidence[i] < minimumConf)
			minimumConf = confidence[i];
	}

	// The first confidence.
	confidence[0] -= minimumConf;

	// Add the minimum confidence to make it nonnegative and 
	// accumulate the probability to get cdf.
	for (int i = 1; i < numParticles; i++) {
		confidence[i] -= minimumConf;
		confidence[i] += confidence[i - 1];
	}

	// Resample.
	resampler = std::uniform_real_distribution<float>(0.0f, confidence[numParticles - 1]);
	curParticle = resampleBuffer;
	for (int i = 0; i < numParticles; i++, curParticle += sizeParticle) {

		// Generate the random number.
		float prob = resampler(generator);

		// Binary search to find the particle.
		int particle = BinarSearch(prob) * sizeParticle;
		
		// Get this particle into resample buffer.
		curParticle[0] = particles[particle];
		curParticle[1] = particles[particle + 1];
	}

	// Swap the particles and resample buffer.
	int *temp = particles;
	particles = resampleBuffer;
	resampleBuffer = temp;
}

void ParticleFilter::DrawParticles(cv::Mat &img, const cv::Scalar &color) const {
	// Draw the particles.
	int *curParticle = particles;
	for (int i = 0; i < numParticles; i++, curParticle += sizeParticle) {
		int x = curParticle[0];
		int y = curParticle[1];
		// Draw a cross.
		cv::line(img, cv::Point(x - 1, y), cv::Point(x + 1, y), color, 2);
		cv::line(img, cv::Point(x, y - 1), cv::Point(x, y + 1), color, 2);
	}
}

void ParticleFilter::DrawTarget(cv::Mat &img, const cv::Scalar &color) const {
	cv::rectangle(img,
		cv::Point(target.upper, target.left),
		cv::Point(target.upper + target.height - 1, target.left + target.width - 1),
		color, 2);
}
