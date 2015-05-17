#include "ParticleFilter.h"

ParticleFilter::ParticleFilter(int n, int szParticle) : numParticles(n), sizeParticle(szParticle) {
	particles = NULL;
	resampleBuffer = NULL;
	confidence = NULL;
}

void ParticleFilter::InitBuffer() {
	particles = new int[numParticles * sizeParticle];
	confidence = new float[numParticles];
	resampleBuffer = new int[numParticles * sizeParticle];
}

void ParticleFilter::InitTarget(const Rect &t) {
	target = t;
	InitParticles();
}

ParticleFilter::~ParticleFilter() {
	if (particles != NULL)
		delete[] particles;
	if (resampleBuffer != NULL)
		delete[] resampleBuffer;
	if (confidence != NULL)
		delete[] confidence;
}

void ParticleFilter::Propagate(const Size &imgSize) {
	int *curParticle = particles;
	for (int i = 0; i < numParticles; i++, curParticle += sizeParticle) {
		while (true) {
			curParticle[0] += (int)gaussian(generator);
			curParticle[1] += (int)gaussian(generator);
			if (imgSize.IsIn(curParticle[0], curParticle[1], target.width, target.height)) {
				break;
			}
		}
	}
}

void ParticleFilter::Observe(const StrongClassifier *classifier, const IntegralImage *intImage) {
	int *curParticle = particles;
	Rect roi(0, 0, target.width, target.height);

	for (int i = 0; i < numParticles; i++, curParticle += sizeParticle) {
		// Set the roi.
		roi.upper = curParticle[0];
		roi.left = curParticle[1];

		// Evaluate the particle.
		confidence[i] = classifier->Evaluate(intImage, roi);
	}
}

void ParticleFilter::Observe(const StrongClassifier *classifier, const IntegralImage *intImage,
	const Rect &detection, float detectionWeight, float classifierWeight) {

	int *curParticle = particles;
	Rect roi(0, 0, target.width, target.height);

	Point2D detectionPoint(detection.upper, detection.left);

	for (int i = 0; i < numParticles; i++, curParticle += sizeParticle) {
		// Set the roi.
		roi.upper = curParticle[0];
		roi.left = curParticle[1];

		Point2D particlePoint(curParticle[0], curParticle[1]);

		// Evaluate the particle.
		confidence[i] = classifierWeight * classifier->Evaluate(intImage, roi);

		// Add the detection term.
		confidence[i] += detectionWeight * GetGaussianProb(0.0f, 
			detection.width * 0.125f, 
			detectionPoint.Distance(particlePoint));
	}
}

void ParticleFilter::ResampleWithBest() {

	// We want to cheat...
	float maxConf = 0.0f;
	int maxParticle = 0;
	for (int i = 0; i < numParticles; i++) {
		if (confidence[i] > maxConf) {
			maxConf = confidence[i];
			maxParticle = i;
		}
	}

	target.upper = particles[maxParticle * sizeParticle];
	target.left = particles[maxParticle * sizeParticle + 1];
	InitParticles();
	
}

void ParticleFilter::ResampleWithConfidence() {
	// Add up all the confidence to get the cdf.
	for (int i = 1; i < numParticles; i++) {
		confidence[i] += confidence[i - 1];
	}

	resampler = std::uniform_real_distribution<float>(0.0f, confidence[numParticles - 1]);
	int *curParticle = resampleBuffer;

	int sumUpper = 0;
	int sumLeft = 0;

	for (int i = 0; i < numParticles; i++, curParticle += sizeParticle) {

		// Generate the random number.
		float prob = resampler(generator);

		// Binary search to find the particle.
		int particle = BinarSearch(prob) * sizeParticle;

		// Get this particle into resample buffer.
		curParticle[0] = particles[particle];
		curParticle[1] = particles[particle + 1];

		// Add it to the sum so that we can get the target later.
		sumUpper += curParticle[0];
		sumLeft += curParticle[1];

	}

	// Swap the particles and resample buffer.
	int *temp = particles;
	particles = resampleBuffer;
	resampleBuffer = temp;

	// Get the new target.
	target.upper = (int)(sumUpper / numParticles);
	target.left = (int)(sumLeft / numParticles);
}

void ParticleFilter::DrawParticles(cv::Mat &img, const cv::Scalar &color) const {
	// Draw the particles.
	int *curParticle = particles;
	for (int i = 0; i < numParticles; i++, curParticle += sizeParticle) {
		int y = curParticle[0];
		int x = curParticle[1];
		// Draw a cross.
		cv::line(img, cv::Point(x - 1, y), cv::Point(x + 1, y), color, 2);
		cv::line(img, cv::Point(x, y - 1), cv::Point(x, y + 1), color, 2);
	}
}

void ParticleFilter::DrawParticlesWithConfidence(cv::Mat &img, const cv::Scalar &color) const {
	float maxConf = 0.0f;
	for (int i = 0; i < numParticles; i++) {
		if (confidence[i] > maxConf) {
			maxConf = confidence[i];
		}
	}

	int *curParticle = particles;
	for (int i = 0; i < numParticles; i++, curParticle += sizeParticle) {
		int y = curParticle[0];
		int x = curParticle[1];
		cv::line(img, cv::Point(x - 1, y), cv::Point(x + 1, y), color * confidence[i] / maxConf, 2);
		cv::line(img, cv::Point(x, y - 1), cv::Point(x, y + 1), color * confidence[i] / maxConf, 2);
	}
}

void ParticleFilter::DrawTarget(cv::Mat &img, const cv::Scalar &color) const {
	cv::rectangle(img,
		cv::Point(target.left, target.upper),
		cv::Point(target.left + target.width - 1, target.upper + target.height - 1),
		color, 2);
}

const Rect &ParticleFilter::GetTarget() const {
	return target;
}

void ParticleFilter::InitParticles() {
	int *curPartice = particles;
	int upper = target.upper;
	int left = target.left;

	// Initialize the gaussian distribution according to the size.
	gaussian = std::normal_distribution<float>(0.0f, target.width * 0.25f);

	for (int i = 0; i < numParticles; i++, curPartice += sizeParticle) {
		curPartice[0] = upper + (int)gaussian(generator);
		curPartice[1] = left + (int)gaussian(generator);
	}
}