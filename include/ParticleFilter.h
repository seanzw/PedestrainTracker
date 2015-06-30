/**
 * A basic particle filter.
 * It just propagates the particles and observe using
 * the strong classifier's evaluate function. We leave
 * the training to upper class.
 * 
 * This class doesn't support virtual function!!!!
 *
 * @author Zhengrong Wang.
 */

#ifndef PARTICLEFILTER_HEADER
#define PARTICLEFILTER_HEADER

#include "StrongClassifier.h"
#include "IntegralImage.h"
#include "MatchMatrix.h"

class ParticleFilter {
public:

	/**
	 * This constructor doesn't initialize particles.
	 * Should call InitializeBuffer and InitializeTarget before actually using it.
	 */
	ParticleFilter(int n = 100, int szParticle = 2);

	~ParticleFilter();

	/**
	 * Initialize the buffer, mainly the particles, confidence, resample buffer.
	 */
	void InitBuffer();

	/**
	 * Initialize the target, used in multiple targets tracking.
	 */
	void InitTarget(const Rect &target);

	/**
	 * Propagate the particles.
	 * 
	 * @param imgSize	Is the particle still inside the image?
	 */
	void Propagate(const Size &imgSize);

	/**
	 * Observe with classifier output confidence.
	 */
	void Observe(const StrongClassifier *classifier, const IntegralImage *intImage);

	/**
	 * Observe with classifier output confidence and associated detection.
	 */
	void Observe(const StrongClassifier *classifier, const IntegralImage *intImage,
		const Rect &detection, float detectionWeight);

	/**
	 * Resample directly around the best particle.
	 */
	void ResampleWithBest();
	
	/**
	 * Resample with the confidence.
	 */
	void ResampleWithConfidence();


	// Draw the particles.
	void DrawParticles(cv::Mat &img, const cv::Scalar &color) const;
	void DrawParticlesWithConfidence(cv::Mat &img, const cv::Scalar &color) const;

	// Draw the target;
	void DrawTarget(cv::Mat &img, const cv::Scalar &color) const;

    inline const Rect &GetTarget() const {
        return target;
    }

    inline void SetTarget(const Rect &t) {
        prevDets.push_back(t);
        if (prevDets.size() == 5) {
            prevDets.pop_front();
        }

        // Set the target size as the average of last four detections.
        int width = 0, height = 0;
        for (const auto &det : prevDets) {
            width += det.width;
            height += det.height;
        }

        target.width = width / prevDets.size();
        target.height = height / prevDets.size();
    }

protected:

	// The sizeof particles. Here it should be 2. Its structure like:
	// struct Particle {
	//		int upper;
	//		int left;
	// }
	const int numParticles;
	int *particles;
	const int sizeParticle;
	
	float *confidence;

	Rect target;

    std::deque<Rect> prevDets;

	// Seed.
	std::default_random_engine generator;

	// Gaussian random generator.
	std::normal_distribution<float> gaussian;

	// Uniform random generator.
	std::uniform_real_distribution<float> resampler;

	// A buffer used in resampling.
	int *resampleBuffer;

	// Binary search, used in resample.
	inline int BinarSearch(float prob) const {
		// A corner case.
		if (prob <= confidence[0])
			return 0;
		int start = 0;
		int end = numParticles - 1;
		int mid = (start + end) / 2;
		while (start < end - 1) {
			if (prob <= confidence[mid]) {
				end = mid;
			}
			else {
				start = mid;
			}
			mid = (start + end) / 2;
		}
		return end;
	}

	/**
	 * Initialize the particles around the target.
	 */
	void InitParticles();

};

#endif