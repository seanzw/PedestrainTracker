#include "ParticleFilterTracker.h"

ParticleFilterTracker::ParticleFilterTracker(StrongClassifier *c, 
	IntegralImage *i, ParticleFilter *p, SingleSampler *s)
	: classifier(c), intImage(i), particleFilter(p), sampler(s) {
}

ParticleFilterTracker::~ParticleFilterTracker() {
}

void ParticleFilterTracker::Track(cv::VideoCapture &in, cv::VideoWriter &out) {
	int width = (int)in.get(cv::CAP_PROP_FRAME_WIDTH);
	int height = (int)in.get(cv::CAP_PROP_FRAME_HEIGHT);

	// Get the total number of images.
	int totalFrames = (int)in.get(cv::CAP_PROP_FRAME_COUNT);
	int count = -1;

	const Size imgSize(width, height);

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

#ifdef USE_RGI_FEATURE
		
		intImage->CalculateInt(frame);

#else

		// Transform this frame into grayscale.
		cv::cvtColor(frame, gray, cv::COLOR_RGB2GRAY);

		// Prepare the integral image.
		intImage->CalculateInt(gray);

#endif

		// Propagate the particles.
		particleFilter->Propagate(imgSize);

		// Make the observation.
		particleFilter->Observe();

		// Draw the particles for debugging.
		particleFilter->DrawParticlesWithConfidence(frame, cv::Scalar(255.0f));
		cv::imshow("particles", frame);
		cv::imwrite("ParticlesConfidence.jpg", frame);
		cv::waitKey();

		particleFilter->Resample();

		// Draw the particles for debugging.
		particleFilter->DrawParticles(frame, cv::Scalar(0.0f, 0.0f, 255.0f));
		cv::imshow("particles", frame);
		cv::waitKey();

		// Draw the target back into frame.
		particleFilter->DrawTarget(frame, cv::Scalar(0.0f, 255.0f, 0.0f));
		cv::imshow("target", frame);
		cv::waitKey();

		// Use the new target to sample.
		sampler->Sample(particleFilter->GetTarget(), imgSize);

		/*
		// Train the online boosting classifier.
		for (int i = 0; i < sampler->GetNumPos(); i++) {
			classifier->Update(intImage, sampler->GetPosSample(i), 1);
		}

		for (int i = 0; i < sampler->GetNumNeg(); i++) {
			classifier->Update(intImage, sampler->GetNegSample(i), -1);
		}
		*/

		// Write back the result into video.
		out.write(frame);
	}

	// Release the data buffer.
	frame.release();
	gray.release();
}