#include "SingleSampler.h"

std::default_random_engine SingleSampler::generator;

SingleSampler::SingleSampler(int numPos, int numNeg) : numPosSamples(numPos), numNegSamples(numNeg) {
	negSamples = new Rect[numNegSamples];
	posSamples = new Rect[numPosSamples];
}

SingleSampler::~SingleSampler() {
	delete[] negSamples;
}

const Rect &SingleSampler::GetNegSample(int index) const {
	return negSamples[index];
}

const Rect &SingleSampler::GetPosSample(int index) const {
	return posSamples[index];
}

int SingleSampler::GetNumPos() const {
	return numPosSamples;
}

int SingleSampler::GetNumNeg() const {
	return numNegSamples;
}

void SingleSampler::Sample(const Rect &pos, const Size &imgSize) {
	gaussianWidth = std::normal_distribution<float>(pos.width / 4.0f, pos.width / 4.0f);
	gaussianHeight = std::normal_distribution<float>(pos.height / 4.0f, pos.height / 4.0f);
	
	// Get the first pos sample.
	posSamples[0] = pos;

	// Sampler positive samples.
	for (int i = 1; i < numPosSamples; i++) {
		posSamples[i].height = pos.height;
		posSamples[i].width = pos.width;
		while (true) {
			int offsetHeight = (int)gaussianHeight(generator) * ((rand() % 2) * 2 - 1);
			int offsetWidth = (int)gaussianWidth(generator) * ((rand() % 2) * 2 - 1);
			posSamples[i].left = pos.left + offsetWidth;
			posSamples[i].upper = pos.upper + offsetHeight;
			if (imgSize.IsIn(posSamples[i])) {
				break;
			}
		}
	}

	gaussianWidth = std::normal_distribution<float>(pos.width, pos.width / 4.0f);
	gaussianHeight = std::normal_distribution<float>(pos.height, pos.height / 4.0f);

	// Sampler negative samples.
	for (int i = 0; i < numNegSamples; i++) {
		negSamples[i].height = pos.height;
		negSamples[i].width = pos.width;
		while (true) {
			int offsetHeight = (int)gaussianHeight(generator) * ((rand() % 2) * 2 - 1);
			int offsetWidth = (int)gaussianWidth(generator) * ((rand() % 2) * 2 - 1);
			negSamples[i].left = pos.left + offsetWidth;
			negSamples[i].upper = pos.upper + offsetHeight;
			if (imgSize.IsIn(negSamples[i])) {
				break;
			}
		}
	}
}

void SingleSampler::DrawSamples(cv::Mat &img, const cv::Scalar &posColor, const cv::Scalar &negColor) const {
	for (int i = 0; i < numPosSamples; i++) {
		cv::rectangle(img, (cv::Rect)posSamples[i], posColor, 2);
	}
	for (int i = 0; i < numNegSamples; i++) {
		cv::rectangle(img, (cv::Rect)negSamples[i], negColor, 2);
	}
}