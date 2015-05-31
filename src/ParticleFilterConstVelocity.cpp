#include "ParticleFilterConstVelocity.h"

ParticleFilterConstVelocity::ParticleFilterConstVelocity(int n, float vThre, float dWeight)
	: ParticleFilter(n, 4), velocityThre(vThre), distWeight(dWeight) {

}

void ParticleFilterConstVelocity::InitBuffer() {
	particles = new int[numParticles * sizeParticle];
	confidence = new float[numParticles];
	resampleBuffer = new int[numParticles * sizeParticle];
}

void ParticleFilterConstVelocity::InitTarget(const Rect &t, const Point2D &v) {
	target = t;
	velocity = v;
	InitParticles();
}

void ParticleFilterConstVelocity::InitParticles() {

	int *curParticle = particles;
	int upper = target.upper;
	int left = target.left;

	// Initialize the gaussian distribution according to the size.
	gaussian = std::normal_distribution<float>(0.0f, target.width * 0.125f);

	for (int i = 0; i < numParticles; i++, curParticle += sizeParticle) {
		curParticle[0] = upper + (int)gaussian(generator);
		curParticle[1] = left + (int)gaussian(generator);
		curParticle[2] = velocity.row;
		curParticle[3] = velocity.col;
	}
}

ParticleFilterConstVelocity::~ParticleFilterConstVelocity() {
	// ~ParticleFilter should delete all stuff.
}

void ParticleFilterConstVelocity::Propagate(const Size &imgSize) {
	int *curParticle = particles;
	for (int i = 0; i < numParticles; i++, curParticle += sizeParticle) {
		while (true) {
			curParticle[0] += (curParticle[2] + (int)gaussian(generator));
			curParticle[1] += (curParticle[3] + (int)gaussian(generator));
			if (imgSize.IsIn(curParticle[0], curParticle[1], target.width, target.height)) {
				curParticle[2] += (int)gaussianVelocity(generator);
				curParticle[3] += (int)gaussianVelocity(generator);
				break;
			}
		}
	}
}

void ParticleFilterConstVelocity::SetVelocitySigma(float sigma) {
	gaussianVelocity = std::normal_distribution<float>(0.0f, sigma);
}

void ParticleFilterConstVelocity::CalculateMatchScore(const IntegralImage *intImage,
	const StrongClassifier *classifier,
	const Pool<Rect> &dets,
	std::vector<MatchMatrix::MatchScore>::iterator &matchArray) const {
	
	Point2D targetPoint(target.upper, target.left);

	for (int i = 0; i < dets.size; i++) {

		// First get the classifier score.
		float classifierScore = classifier->Evaluate(intImage, dets[i]);

		// Then get all the particle-detection distance score.
		float distanceScore = 0.0f;
		Point2D detectionPoint(dets[i].upper, dets[i].left);
		int *curParticle = particles;
		for (int j = 0; j < numParticles; j++, curParticle += sizeParticle) {
			Point2D particlePoint(curParticle[0], curParticle[1]);
			distanceScore += GetGaussianProb(0.0f,
				dets[i].width * 0.125f,
				detectionPoint.Distance(particlePoint));
		}

		// Finally we calculate the gate function.
		// Gate function 1: size term;
		float sizeScore = GetGaussianProb(0.0f,
			dets[i].width * 0.125f,
			dets[i].width - target.width);
		sizeScore *= GetGaussianProb(0.0f, 
			dets[i].height * 0.125f,
			dets[i].height - target.height);

		// Gate function 2: velocity term;
		float velocityScore = 0.0f;
		if (velocity.SquaredLength() > velocityThre) {
			// The velocity is big enough.
			float residual = velocity.ProjectResidual(detectionPoint - targetPoint);
			velocityScore = GetGaussianProb(0.0f,
				detectionPoint.Distance(targetPoint),
				residual);
		}
		else {
			// This target is almost not moving.
			// Consider the distance instead.
			velocityScore = GetGaussianProb(0.0f,
				dets[i].width * 1.0f,
				detectionPoint.Distance(targetPoint));
		}

		// Get all pieces together.
		float matchScore = 1000.0f * velocityScore * (classifierScore + distWeight * distanceScore + sizeScore);

#ifdef PFCV_DEBUG
		printf("===== Calculating matching score ======\n");
		printf("Classifier Score  |  Distance Score  |  Size Score  |  Velocity Score\n");
		printf("  %14f  |  %14f  |  %10f  |  %10f\n", classifierScore, distanceScore, sizeScore, velocityScore);
		printf("Distance Weight = %f\n", distWeight);
		printf("MatchScore = %f\n", matchScore);
		printf("===== Finish Print Matching Score =====\n\n");
#endif

		// Set the match score.
		matchArray[i].score = matchScore;
	}
}