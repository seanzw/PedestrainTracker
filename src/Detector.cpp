#include "Detector.h"
/*
ImageDetector::ImageDetector(FeatureExtractor &fe, Classifier &c, Options &op) {
	featureExt = fe;
	classifier = c;
	scaleMin = op.scaleMin;
	scaleMax = op.scaleMax;
	scaleStep = op.scaleStep;
	slideStep = op.slideStep;
	evidence = op.evidence;
	modelWidth = op.modelWidth;
	modelHeight = op.modelHeight;
}

void ImageDetector::Detect(const cv::Mat &img, const cv::Mat &imgGray) const {
	// Calculate the HoG integral image.
	featureExt.Preprocess(imgGray);

	// Slide the window and detect.
	for (feat scale = scaleMin; scale < scaleMax; scale *= scaleStep) {
		for (int i = 0; i <= img.size().height - modelHeight * scale; i = i + (int)(slideStep * scale)) {
			for (int j = 0; j <= img.size().width - modelWidth * scale; j = j + (int)(slideStep * scale)) {
				if (classifier.Classify(i, j, scale, featureExt)) {

				}
			}
		}
	}
}
*/