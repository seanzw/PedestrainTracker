#include "HaarFeature.h"

#define SQRTHALF 0.7071
#define INITSIGMA(numAreas) (static_cast<float>(sqrtf(256.0f * 256.0f / 12.0f * (numAreas))))

// All the five types has the same probability.
const float HaarFeature::cdf[5] = { 0.2, 0.4, 0.6, 0.8, 1.0 };

// Define the minimum size and area.
const Size HaarFeature::minimumSize = Size(3, 3);
const int HaarFeature::minimumArea = 9;

HaarFeature::HaarFeature(const Size &imageSize) {
	try {
		GenerateRandomFeature(imageSize);
	}
	catch (...) {
		delete[] scaledWeights;
		delete[] scaledAreas;
		delete[] areas;
		delete[] weights;
	}
}

HaarFeature::~HaarFeature() {
	delete[] scaledWeights;
	delete[] scaledAreas;
	delete[] areas;
	delete[] weights;
}

void HaarFeature::GenerateRandomFeature(const Size &imageSize) {

	Point2D upperLeftCorner;
	Size baseDim;
	Size sizeFactor;
	int area;

	bool valid = false;
	while (!valid) {
		
		// Choose upperLeftCorner and scale.
		upperLeftCorner.row = rand() % imageSize.height;
		upperLeftCorner.col = rand() % imageSize.width;

		baseDim.width = (int)((1 - sqrtf(1 - (float)rand() / RAND_MAX)) * imageSize.width);
		baseDim.height = (int)((1 - sqrtf(1 - (float)rand() / RAND_MAX)) * imageSize.height);

		// Use the CDF to select the type.
		float prob = (float)rand() / RAND_MAX;
		type = 0;
		while (prob > cdf[type]) {
			type++;
		}

		switch (type) {
		case(0):
			// Check if the feature is valid.
			sizeFactor.height = 2;
			sizeFactor.width = 1;
			if (upperLeftCorner.row + baseDim.height*sizeFactor.height >= imageSize.height ||
				upperLeftCorner.col + baseDim.width*sizeFactor.width >= imageSize.width)
				continue;
			area = baseDim.height*sizeFactor.height*baseDim.width*sizeFactor.width;
			if (area < minimumArea)
				continue;

			numAreas = 2;
			weights = new int[numAreas];
			weights[0] = 1;
			weights[1] = -1;
			areas = new Rect[numAreas];
			areas[0].left = upperLeftCorner.col;
			areas[0].upper = upperLeftCorner.row;
			areas[0].height = baseDim.height;
			areas[0].width = baseDim.width;
			areas[1].left = upperLeftCorner.col;
			areas[1].upper = upperLeftCorner.row + baseDim.height;
			areas[1].height = baseDim.height;
			areas[1].width = baseDim.width;
			initMean = 0;
			initSigma = INITSIGMA(numAreas);

			valid = true;
			break;

		case(1):
			//check if feature is valid
			sizeFactor.height = 1;
			sizeFactor.width = 2;
			if (upperLeftCorner.row + baseDim.height*sizeFactor.height >= imageSize.height ||
				upperLeftCorner.col + baseDim.width*sizeFactor.width >= imageSize.width)
				continue;
			area = baseDim.height*sizeFactor.height*baseDim.width*sizeFactor.width;
			if (area < minimumArea)
				continue;

			numAreas = 2;
			weights = new int[numAreas];
			weights[0] = 1;
			weights[1] = -1;
			areas = new Rect[numAreas];
			areas[0].left = upperLeftCorner.col;
			areas[0].upper = upperLeftCorner.row;
			areas[0].height = baseDim.height;
			areas[0].width = baseDim.width;
			areas[1].left = upperLeftCorner.col + baseDim.width;
			areas[1].upper = upperLeftCorner.row;
			areas[1].height = baseDim.height;
			areas[1].width = baseDim.width;
			initMean = 0;
			initSigma = INITSIGMA(numAreas);
			valid = true;

		case(2):
			//check if feature is valid
			sizeFactor.height = 4;
			sizeFactor.width = 1;
			if (upperLeftCorner.row + baseDim.height*sizeFactor.height >= imageSize.height ||
				upperLeftCorner.col + baseDim.width*sizeFactor.width >= imageSize.width)
				continue;
			area = baseDim.height*sizeFactor.height*baseDim.width*sizeFactor.width;
			if (area < minimumArea)
				continue;

			numAreas = 3;
			weights = new int[numAreas];
			weights[0] = 1;
			weights[1] = -2;
			weights[2] = 1;
			areas = new Rect[numAreas];
			areas[0].left = upperLeftCorner.col;
			areas[0].upper = upperLeftCorner.row;
			areas[0].height = baseDim.height;
			areas[0].width = baseDim.width;
			areas[1].left = upperLeftCorner.col;
			areas[1].upper = upperLeftCorner.row + baseDim.height;
			areas[1].height = 2 * baseDim.height;
			areas[1].width = baseDim.width;
			areas[2].upper = upperLeftCorner.row + 3 * baseDim.height;
			areas[2].left = upperLeftCorner.col;
			areas[2].height = baseDim.height;
			areas[2].width = baseDim.width;
			initMean = 0;
			initSigma = INITSIGMA(numAreas);
			valid = true;
			break;

		case(3):
			sizeFactor.height = 1;
			sizeFactor.width = 4;
			if (upperLeftCorner.row + baseDim.height*sizeFactor.height >= imageSize.height ||
				upperLeftCorner.col + baseDim.width*sizeFactor.width >= imageSize.width)
				continue;
			area = baseDim.height*sizeFactor.height*baseDim.width*sizeFactor.width;
			if (area < minimumArea)
				continue;

			numAreas = 3;
			weights = new int[numAreas];
			weights[0] = 1;
			weights[1] = -2;
			weights[2] = 1;
			areas = new Rect[numAreas];
			areas[0].left = upperLeftCorner.col;
			areas[0].upper = upperLeftCorner.row;
			areas[0].height = baseDim.height;
			areas[0].width = baseDim.width;
			areas[1].left = upperLeftCorner.col + baseDim.width;
			areas[1].upper = upperLeftCorner.row;
			areas[1].height = baseDim.height;
			areas[1].width = 2 * baseDim.width;
			areas[2].upper = upperLeftCorner.row;
			areas[2].left = upperLeftCorner.col + 3 * baseDim.width;
			areas[2].height = baseDim.height;
			areas[2].width = baseDim.width;
			initMean = 0;
			initSigma = INITSIGMA(numAreas);
			valid = true;
			break;
		case(4):
			sizeFactor.height = 2;
			sizeFactor.width = 2;
			if (upperLeftCorner.row + baseDim.height*sizeFactor.height >= imageSize.height ||
				upperLeftCorner.col + baseDim.width*sizeFactor.width >= imageSize.width)
				continue;
			area = baseDim.height*sizeFactor.height*baseDim.width*sizeFactor.width;
			if (area < minimumArea)
				continue;

			numAreas = 4;
			weights = new int[numAreas];
			weights[0] = 1;
			weights[1] = -1;
			weights[2] = -1;
			weights[3] = 1;
			areas = new Rect[numAreas];
			areas[0].left = upperLeftCorner.col;
			areas[0].upper = upperLeftCorner.row;
			areas[0].height = baseDim.height;
			areas[0].width = baseDim.width;
			areas[1].left = upperLeftCorner.col + baseDim.width;
			areas[1].upper = upperLeftCorner.row;
			areas[1].height = baseDim.height;
			areas[1].width = baseDim.width;
			areas[2].upper = upperLeftCorner.row + baseDim.height;
			areas[2].left = upperLeftCorner.col;
			areas[2].height = baseDim.height;
			areas[2].width = baseDim.width;
			areas[3].upper = upperLeftCorner.row + baseDim.height;
			areas[3].left = upperLeftCorner.col + baseDim.width;
			areas[3].height = baseDim.height;
			areas[3].width = baseDim.width;
			initMean = 0;
			initSigma = INITSIGMA(numAreas);
			valid = true;
			break;

		default:
			printf("The probility is wrong! prob: %f\n", prob);
			exit(0);
			break;
		}
	}

	initSize = imageSize;
	curSize = initSize;
	scaledWidth = scaledHeight = 1.0f;
	scaledAreas = new Rect[numAreas];
	scaledWeights = new float[numAreas];

	for (int i = 0; i < numAreas; i++) {
		scaledAreas[i] = areas[i];
		scaledWeights[i] = (float)weights[i] / (float)(areas[i].width * areas[i].height);
	}
}

void HaarFeature::GetInitialDistribution(EstimatedGaussianDistribution<1> *distribution) const {
	distribution->SetValues(&initMean, &initSigma);
}

bool HaarFeature::Extract(const IntegralImage *intImage, const Rect &roi, Feature *feature) {
	feature->Resize(1);
	feature->data[0] = 0.0f;

	Point2D offset;
	offset = roi;

	if (curSize.width != roi.width || curSize.height != roi.height) {
		// We need to resize it.
		curSize = roi;
		if (!(initSize == curSize)) {
			scaledWidth = (float)curSize.width / initSize.width;
			scaledHeight = (float)curSize.height / initSize.height;

			for (int i = 0; i < numAreas; i++) {
				scaledAreas[i].width = floorf((float)areas[i].width * scaledWidth + 0.5);
				scaledAreas[i].height = floorf((float)areas[i].height * scaledHeight + 0.5);

				if (scaledAreas[i].height < minimumSize.height || scaledAreas[i].width < minimumSize.width) {
					// After rescaling, the patch is too small, the feature is invalid.
					scaledWidth = 0.0f;
					return false;
				}

				scaledAreas[i].left = floor((float)areas[i].left * scaledWidth + 0.5);
				scaledAreas[i].upper = floor((float)areas[i].upper * scaledHeight + 0.5);

				scaledWeights[i] = (float)weights[i] / (float)(scaledAreas[i].width * scaledAreas[i].height);
			}
		}
		else {
			// The new patch is the same as the original size.
			scaledWidth = scaledHeight = 1.0f;
			for (int i = 0; i < numAreas; i++) {
				scaledAreas[i] = areas[i];
				scaledWeights[i] = (float)weights[i] / (float)(areas[i].width * areas[i].height);
			}
		}
	}

	// Finish rescaling.
	if (scaledWidth == 0.0f) {
		return false;
	}

	for (int i = 0; i < numAreas; i++) {
		feature->data[0] += (float)intImage->GetSum(scaledAreas[i] + offset) * scaledWeights[i];
	}

	response = feature->data[0];

	return true;
}