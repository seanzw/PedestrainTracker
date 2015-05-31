#include "MultiTracker.h"

MultiTracker::MultiTracker(ImageDetector *d, const Size &sz, const Options &opts)
	: imgSize(sz), detector(d), matchThre(opts.matchThre) {

	// Initialize the integral images.
	hogIntImage = new HoGIntegralImage(imgSize.width, imgSize.height);
	rgiIntImage = new RGIIntegralImage(imgSize.width, imgSize.height);

	// Initialize the targets free list.
	targets = new TargetsFreeList(opts);

	// Initialize the sampler.
	sampler = new MultiSampler(opts);

	// Construct the match matrix.
	matches = new MatchMatrix(opts.targetsFreeListCapacity);

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

		// Remember to clear all the detections.
		detector->Clear();

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
		targets->CalculateMatchScore(rgiIntImage, detector->dets, matches->matchMatrix);

#ifdef MT_DEBUG

		// Print the match matrix for debugging.
		matches->PrintMatchMatrix();
#endif

		// Find match pair.
		matches->SetTargets(targets->matchDets, matchThre);

#ifdef MT_DEBUG

		// Draw all the detections for debugging.
		cv::Mat detections = frame.clone();
		detector->DrawDetection(detections);
		cv::imshow("detections", detections);

		// Draw all the targets for debugging.
		cv::Mat tars = frame.clone();
		targets->DrawTargets(tars);
		cv::imshow("targets", tars);

		// Draw all the match pair for debugging.
		cv::Mat mats = frame.clone();
		targets->DrawMatches(mats, detector->dets);
		cv::imshow("matches", mats);

		cv::waitKey();

#endif

		// Inititalize a target for every unmatched detection.
		for (int i = 0; i < detector->dets.size; i++) {
			if (matches->isDetMatched[i] == 0) {
				// This detection is unmatched.
				if (count == 0) {
					// This is the first frame, initialize everything.
					int id = targets->InitializeTarget(detector->dets[i], Point2D(0, 0));
					matches->isDetMatched[i] = id;
					targets->matchDets[id] = i;
				}
				else {
					// We have to check if there is any target nearby.
					if (!targets->CheckNearbyTarget(detector->dets[i], 20)) {
						// There are no nearby target.
						// We believe this is a new target.
						int id = targets->InitializeTarget(detector->dets[i], Point2D(0, 0));
						matches->isDetMatched[i] = id;
						targets->matchDets[id] = i;
					}
				}
			}
		}

		// Make the observation.
		targets->Observe(rgiIntImage, detector->dets);

		// Resample.
		targets->Resample();

		// Sample around the match pair.
		sampler->Sample(targets->GetMatchDets(), detector->dets, imgSize);

		// Online training.
		targets->Train(rgiIntImage, sampler);

#ifdef MT_DEBUG

		

#endif


		//// Draw the particles for debugging.
		//// particleFilter->DrawParticlesWithConfidence(frame, cv::Scalar(255.0f));
		//cv::imshow("particles", frame);
		//cv::imwrite("ParticlesConfidence.jpg", frame);
		//cv::waitKey();

		////particleFilter->ResampleWithBest();

		//// Draw the particles for debugging.
		////particleFilter->DrawParticles(frame, cv::Scalar(0.0f, 0.0f, 255.0f));
		//cv::imshow("particles", frame);
		//cv::waitKey();

		//// Draw the target back into frame.
		////particleFilter->DrawTarget(frame, cv::Scalar(0.0f, 255.0f, 0.0f));
		//cv::imshow("target", frame);
		//cv::waitKey();

		// Write back the result into video.
		out.write(frame);
	}

	// Release the data buffer.
	frame.release();
	gray.release();
}