#include "VideoDetector.h"

VideoDetector::VideoDetector(IntegralImage *i, ImageDetector *id, const Options &op)
	: imgDetector(id), intImage(i) {

}

void VideoDetector::Detect(cv::VideoCapture &in, 
	cv::VideoWriter &out, 
	const cv::Mat &bkg) {
	int width = in.get(cv::CAP_PROP_FRAME_WIDTH);
	int height = in.get(cv::CAP_PROP_FRAME_HEIGHT);

	// Get the total number of images.
	int totalFrames = in.get(cv::CAP_PROP_FRAME_COUNT);
	int count = -1;

	// Create the data buffer.
	cv::Mat frame(cv::Size(width, height), CV_8UC3);
	cv::Mat gray(cv::Size(width, height), CV_8UC1);

	// Set the sub-region the whole image.
	Rect subRegion(0, 0, width, height);

	// Read all the frames.
	while (in.read(frame)) {

		count++;

		// Detect every two frames.
		if (count % 2) {
			continue;
		}

		VDPRINTF("Processing Frame: %d / %d, %.1f%%\n", count, totalFrames, 100.0 * count / totalFrames);
		VDPRINTF("=====================================\n");

		// Transform this frame into grayscale.
		cv::cvtColor(frame, gray, cv::COLOR_RGB2GRAY);

		// Remember to clear the detection first.
		imgDetector->Clear();

		// Calculate the integral image.
		intImage->CalculateInt(gray);

		// To the detection.
		imgDetector->Detect(gray, intImage, subRegion, bkg);

		// Draw the results back into frame.
		imgDetector->DrawDetection(frame); 

		// Write back the result into video.
		out.write(frame);
	}

	// Release the data buffer.
	frame.release();
	gray.release();
}