#include "SingleTarget.h"

SingleTarget::SingleTarget(const Options &opts)
	: detectionSeq(0), velocitySigmaConst(opts.velocitySigmaConst) {

	// Initialize the strong classifier.
	classifier = new StrongClassifierDirectSelect(opts.numSelectors,
		opts.numWeakClassifiers,
		(Size)opts.target,
		opts.numBackups);

	// Construct the particle filter.
	particleFilter = new ParticleFilterConstVelocity(opts.nParticles,
		opts.velocityThre,
		opts.distWeight);
	particleFilter->InitBuffer();
}

SingleTarget::~SingleTarget() {
	delete classifier;
	delete particleFilter;
}

void SingleTarget::InitializeTarget(const Rect &target, const Point2D &initVelocity) {
	
	// Initialize the strong classifier.
	classifier->Initialize((Size)target);

	// Initialize the particle filter.
	particleFilter->InitTarget(target, initVelocity);

	// Reset the detection sequence.
	detectionSeq = 0;
}

void SingleTarget::Propagate(const Size &imgSize) {
	
	// First we need to set the sigma for velocity
	// according to how many frames we have successfully detected.
	float velocitySigma;
	if (detectionSeq == 0) {
		velocitySigma = SINGLE_TARGET_VELOCITY_CONST;
	}
	else {
		velocitySigma = SINGLE_TARGET_VELOCITY_CONST / (float)detectionSeq;
	}
	particleFilter->SetVelocitySigma(velocitySigma);

	// Propagate the particles.
	particleFilter->Propagate(imgSize);
}

void SingleTarget::Train(const IntegralImage *intImage, const MultiSampler *multiSampler, int id) {

#ifdef ST_DEBUG

	// Visualize the training process for debugging.
	// First let's try to show the integral image.
	cv::Mat img(intImage->height, intImage->width, CV_8UC3);

	// Draw the target for debugging.
	particleFilter->DrawTarget(img, cv::Scalar(255.0f, 255.0f, 255.0f));

#endif

	// Update the classifier.
	for (MultiSampler::const_iterator iter = multiSampler->begin(id);
		iter != multiSampler->end();
		++iter) {
		classifier->Update(intImage, *iter, iter.GetTarget(), 1.0f);

#ifdef ST_DEBUG
		
		// Get the score after training.
		float score = classifier->Evaluate(intImage, *iter);

		// Draw the training sample for debugging.
		cv::Scalar color;
		if (iter.GetTarget() == 1) {
			// This is a positive trainging sample.
			// We draw it with green.
			color = cv::Scalar(0.0f, 255.5f, 0.0f);

			// Print the score after training.
			printf("Positive sample: %f\n", score);
		}
		else {
			// This is a negative training sample.
			// We draw it with red.
			color = cv::Scalar(0.0f, 0.0f, 255.0f);

			// Print the score after training.
			printf("Negative sample: %f\n", score);
		}

		cv::rectangle(img, (cv::Rect)*iter, color, 2);
		//cv::imshow("training", img);
		//cv::waitKey();

#endif


	}
}

void SingleTarget::CalculateMatchScore(const IntegralImage *intImage,
	const Pool<Rect> &dets,
	std::vector<MatchMatrix::MatchScore>::iterator &matchArray) const {

	particleFilter->CalculateMatchScore(intImage, classifier, dets, matchArray);
}

bool SingleTarget::CheckNearbyTarget(const Rect &det, int distThre) const {
	const Rect &target = particleFilter->GetTarget();
	return target.IsOverlap(det);
}

