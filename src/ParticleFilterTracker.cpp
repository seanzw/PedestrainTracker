#include "ParticleFilterTracker.h"

ParticleFilterTracker::ParticleFilterTracker(StrongClassifier *c, IntegralImage *i, ParticleFilter *p)
	: classifier(c), intImage(i), particleFilter(p) {

}

ParticleFilterTracker::~ParticleFilterTracker() {

}

void ParticleFilterTracker::Track(cv::VideoCapture &in, cv::VideoWriter &out) {
	int width = in.get(cv::CAP_PROP_FRAME_WIDTH);
	int height = in.get(cv::CAP_PROP_FRAME_HEIGHT);

	// Get the total number of images.
	int totalFrames = in.get(cv::CAP_PROP_FRAME_COUNT);
	int count = -1;

	// Create the data buffer.
	cv::Mat frame(cv::Size(width, height), CV_8UC3);
	cv::Mat gray(cv::Size(width, height), CV_8UC1);

	// Read all the frames.
	while (in.read(frame)) {

		count++;

		// Detect every two frames.
		if (count % 2) {
			continue;
		}

		PTPRINTF("Processing Frame: %d / %d, %.1f%%\n", count, totalFrames, 100.0 * count / totalFrames);
		PTPRINTF("=====================================\n");

		// Transform this frame into grayscale.
		cv::cvtColor(frame, gray, cv::COLOR_RGB2GRAY);

		// Prepare the integral image.
		intImage->CalculateInt(gray);

		// Propagate the particles.
		particleFilter->Propagate();

		// Draw the particles for debugging.
		particleFilter->DrawParticles(frame, cv::Scalar(255.0f));
		//cv::imshow("particles", frame);
		//cv::waitKey();


		// Make the observation.
		particleFilter->Observe();

		// Draw the target back into frame.
		particleFilter->DrawTarget(frame, cv::Scalar(0.0f, 1.0f, 0.0f, 1.0f));

		// Write back the result into video.
		out.write(frame);
	}

	// Release the data buffer.
	frame.release();
	gray.release();
}