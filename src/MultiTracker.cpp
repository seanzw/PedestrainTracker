#include "MultiTracker.h"

MultiTracker::MultiTracker(ImageDetector *d, const Size &sz, const Options &opts)
	: imgSize(sz), detector(d), matchThre(opts.matchThre) {

	// Initialize the integral images.
	hogIntImage = new HoGIntegralImage(imgSize.width, imgSize.height);
	rgiIntImage = new RGIIntegralImage(imgSize.width, imgSize.height);

    // Initialize the region.
    int margin = 50;
    outer = Rect(margin, margin, imgSize.width - 2 * margin, imgSize.height - 2 * margin);
    inner = Rect(2 * margin, 2 * margin, imgSize.width - 4 * margin, imgSize.height - 4 * margin);

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

    // Reset the count number;
    curNumPedestrains = 0;
    totalNumPedestrains = 0;

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

        // Draw the region.
        // White for inner, black for outer.
        cv::rectangle(frame, (cv::Rect)inner, cv::Scalar(255.0f, 255.0f, 255.0f), 2);
        cv::rectangle(frame, (cv::Rect)outer, cv::Scalar(0.0f, 0.0f, 0.0f), 2);

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

        // Do the control logic.
        Control(count);

		// Make the observation.
		targets->Observe(rgiIntImage, detector->dets);

#ifdef MT_DEBUG

		// After observation, draw particles with confidence for debugging.
		targets->DrawParticlesWithConfidence(tars);
		cv::imshow("targets", tars);
		cv::waitKey();

#endif

        // Reset those targets not inside the outer.
        Reset();

		// Resample.
		targets->Resample();

#ifdef MT_DEBUG

		// After resample, draw particles and targets for debugging.
		targets->DrawParticles(tars);
		targets->DrawTargets(tars);
		cv::imshow("targets", tars);
		cv::waitKey();

#endif

		// Sample around the match pair.
		sampler->Sample(targets->GetMatchDets(), detector->dets, imgSize);

		// Online training.
		targets->Train(rgiIntImage, sampler);

#ifdef MT_DEBUG

		

#endif

        // Write the count number back.
        cv::putText(frame,
            std::to_string(curNumPedestrains),
            cv::Point(100, 100),
            cv::FONT_HERSHEY_SIMPLEX,
            1.0f,
            cv::Scalar(0.0f),
            2);
        cv::putText(frame,
            std::to_string(totalNumPedestrains),
            cv::Point(150, 100),
            cv::FONT_HERSHEY_SIMPLEX,
            1.0f,
            cv::Scalar(0.0f),
            2);
        targets->DrawTargets(frame);

#ifdef MT_DEBUG
        cv::imshow("result", frame);
        cv::waitKey();
#endif
		// Write back the result into video.
		out.write(frame);
	}

	// Release the data buffer.
	frame.release();
	gray.release();
}

void MultiTracker::Control(int curFrame) {

    // Inititalize a target for every unmatched detection.
    for (int i = 0; i < detector->dets.size; i++) {
        if (matches->isDetMatched[i] == 0) {

            // This detection is unmatched.
            // Check if there is nearby target.
            if (!targets->CheckNearbyTarget(detector->dets[i], 20)) {

                // There is no nearby target.
                if (curFrame < 30) {

                    // We are still in the initialization stage.
                    // Initialize if the detection is inside outer.
                    if (outer.IsIn(detector->dets[i])) {
                        int id = targets->InitializeTarget(detector->dets[i], Point2D(0, 0));
                        matches->isDetMatched[i] = id;
                        targets->matchDets[id] = i;

                        // Increase the count.
                        curNumPedestrains++;
                        totalNumPedestrains++;

                    }
                }
                else {

                    // Normal stage.
                    // Initialize if the detection is inside outer and not inside inner.
                    if (outer.IsIn(detector->dets[i]) && !inner.IsIn(detector->dets[i])) {
                        int id = targets->InitializeTarget(detector->dets[i], Point2D(0, 0));
                        matches->isDetMatched[i] = id;
                        targets->matchDets[id] = i;

                        // Increase the count.
                        curNumPedestrains++;
                        totalNumPedestrains++;

                    }
                }
            }
        }
    }
}

void MultiTracker::Reset() {

    // Release all the target if it's outside the inner.
    Rect t;
    for (int i = 0; i < targets->GetCapacity(); i++) {
        if (targets->GetTarget(i, t)) {
            if (!inner.IsOverlap(t)) {
                targets->ResetOneTarget(i);
                curNumPedestrains--;
            }
        }
    }
}