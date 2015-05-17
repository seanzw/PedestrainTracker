#include "MultiTracker.h"

MultiTracker::MultiTracker(ImageDetector *d, int c, const Size &sz, float thre)
	: imgSize(sz), detector(d), matchThre(thre) {

	// Initialize the integral images.
	hogIntImage = new HoGIntegralImage(imgSize.width, imgSize.height);
	rgiIntImage = new RGIIntegralImage(imgSize.width, imgSize.height);

	// Initialize the targets free list.
	targets = new TargetsFreeList(c, 500, 50, 250, 4, 0.5f);

	// Construct the match matrix.
	matches = new MatchMatrix(c);

}

MultiTracker::~MultiTracker() {
	delete hogIntImage;
	delete rgiIntImage;
	delete targets;
}

void MultiTracker::Track(cv::VideoCapture &in, cv::VideoWriter &out, const cv::Mat &bkg) {

	// Get the total number of images.
	int totalFrames = (int)in.get(cv::CAP_PROP_FRAME_COUNT);
	int count = -1;

	// Create the data buffer.
	cv::Mat frame((cv::Size)imgSize, CV_8UC3);
	cv::Mat gray((cv::Size)imgSize, CV_8UC1);

	Rect subRegion(0, 0, imgSize.width, imgSize.height);

	// Read all the frames.
	while (in.read(frame)) {

		count++;

		// Detect every two frames.
		if (count % 2) {
			continue;
		}

		MTPRINTF("Processing Frame: %d / %d, %.1f%%\n", count, totalFrames, 100.0 * count / totalFrames);
		MTPRINTF("=====================================\n");

		// Convert image into gray.
		cv::cvtColor(frame, gray, cv::COLOR_RGB2GRAY);

		// Calculate all the integral images.
		hogIntImage->CalculateInt(gray);
		rgiIntImage->CalculateInt(frame);

		// Detects.
		detector->Detect(gray, hogIntImage, subRegion, bkg);

		// Propagate the particles.
		targets->Propagate(imgSize);

		// Initialize the match score matrix.
		matches->SetNumDets(detector->dets.size);

		// Calcualate the match score.
		targets->CalculateMatchScore(rgiIntImage, detector->dets, matches->matchMat);

		// Find match pair.
		matches->SetTargets(*targets, matchThre);

		// Make the observation.
		targets->Observe(rgiIntImage, detector->dets);

		// Draw the particles for debugging.
		//particleFilter->DrawParticlesWithConfidence(frame, cv::Scalar(255.0f));
		cv::imshow("particles", frame);
		cv::imwrite("ParticlesConfidence.jpg", frame);
		cv::waitKey();

		//particleFilter->ResampleWithBest();

		// Draw the particles for debugging.
		//particleFilter->DrawParticles(frame, cv::Scalar(0.0f, 0.0f, 255.0f));
		cv::imshow("particles", frame);
		cv::waitKey();

		// Draw the target back into frame.
		//particleFilter->DrawTarget(frame, cv::Scalar(0.0f, 255.0f, 0.0f));
		cv::imshow("target", frame);
		cv::waitKey();

		// Use the new target to sample.
		// sampler->Sample(particleFilter->GetTarget(), imgSize);

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