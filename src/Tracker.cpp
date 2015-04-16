#include "Tracker.h"

// The random generator.
std::default_random_engine generator;

// The size of a particle should be 2;
const int Tracker::sizeParticle = 2;

Tracker::Tracker(const cv::Mat &img, const cv::Rect &det, int n)
	: numParticles(n), imgW(img.size().width), imgH(img.size().height) {

	// Get the position of the detection.
	centerX = det.x + det.width / 2;
	centerY = det.y + det.height / 2;
	width = det.width;
	height = det.height;

	// Initialize the particles.
	InitParticles();

	// Initialize the online-classifier boosting classifier.
	// TO DO:


	// Train it.
	// TO DO:
}

Tracker::~Tracker() {
	delete[] particles;
	delete classifier;
}

void Tracker::Propagate() {
	// TO DO:
}

void Tracker::Observe(const cv::Mat &img) {
	// TO DO:
}

void Tracker::DrawParticles(cv::Mat &img, const cv::Scalar &color) const {
	// Draw the particles.
	int *curParticle = particles;
	for (int i = 0; i < numParticles; i++) {
		int x = curParticle[0];
		int y = curParticle[1];
		// Draw a cross.
		cv::line(img, cv::Point(x - 1, y), cv::Point(x + 1, y), color, 2);
		cv::line(img, cv::Point(x, y - 1), cv::Point(x, y + 1), color, 2);
		// Move the curParticle.
		curParticle += sizeParticle;
	}
}

// Draw the target.
void Tracker::DrawTarget(cv::Mat &img, const cv::Scalar &color) const {
	cv::rectangle(img, 
		cv::Point(centerX - width / 2, centerY - height / 2),
		cv::Point(centerX + width / 2, centerY + height / 2),
		color, 2);
}

void Tracker::InitParticles() {
	particles = new int[sizeParticle * numParticles];

	// Initialize the gaussian distribution according to the size.
	gaussian = std::normal_distribution<float>(0.0f, 5.0f);

	// Draw from Gaussian distribution to get the particles.
	int *curParticle = particles;
	for (int i = 0; i < numParticles; i++) {
		curParticle[0] = centerX + (int)gaussian(generator);
		curParticle[1] = centerY + (int)gaussian(generator);
		curParticle += sizeParticle;
	}
}
