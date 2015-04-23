#include "ImageDetector.h"

ImageDetector::ImageDetector(IntegralImage *i, 
	Classifier *c, Options &op) {
	intImage = i;
	classifier = c;
	scaleMin = op.scaleMin;
	scaleMax = op.scaleMax;
	scaleStep = op.scaleStep;
	slideStep = op.slideStep;
	evidence = op.evidence;
	modelWidth = op.modelWidth;
	modelHeight = op.modelHeight;
}

bool ImageDetector::Detect(const cv::Mat &img, 
	const cv::Point &origin, 
	bool isMerge,
	const cv::Mat &bkg
	) {
	// Calculate the integral image.
	intImage->CalculateInt(img);

	// Set the vector.
	Pool<rect> &dst = isMerge ? (temp) : (dets);

	// Clear.
	temp.clear();


	Rect roi;
	// Slide the window and detect.
	for (feat scale = scaleMin; scale < scaleMax; scale *= scaleStep) {
		roi.width = scale * modelWidth;
		roi.height = scale * modelHeight;
		for (int i = 0; i <= img.size().height - modelHeight * scale; i = i + (int)(slideStep * scale)) {
			for (int j = 0; j <= img.size().width - modelWidth * scale; j = j + (int)(slideStep * scale)) {
				roi.upper = i;
				roi.left = j;
				if (classifier->Classify(intImage, roi, scale)) {
					dst.Push(rect(j + origin.x, 
						i + origin.y, 
						j + (int)(roi.width) + origin.x,
						i + (int)(roi.height) + origin.y));
				}
			}
		}
	}

	// Merge the detections.
	if (isMerge) {
		if (evidence > 1) {

			// Union.
			for (int i = 0; i < dst.size; i++) {
				for (int j = i; j < dst.size; j++) {
					if (IsEqual(dst[i], dst[j])) {
						if (dst[i].re == -1) {
							dst[i].re = dst[j].re = i;
						} else {
							dst[j].re = dst[i].re;
						}
					}
				}
			}

			// Count.
			for (int i = 0; i < dst.size; i++) {
				int count = 0;
				for (int j = i; j < dst.size; j++) {
					if (dst[i].re == i && dst[j].re == i) 
						count++;
				}

				if (dst[i].re == i) 
					dst[i].count = count;

				for (int j = i; j < dst.size; j++) {
					if (dst[i].re == i && dst[j].re == i)
						dst[j].count = count;
				}
			}
		}

		// Merge them...
		for (int i = 0; i < dst.size; i++)
		{
			// This one has no friends...
			if (dst[i].re == -1) 
				continue;

			if (dst[i].re >= 0 && dst[i].count >= evidence) {
				rect t(0, 0, 0, 0);

				for (int j = i; j < dst.size; j++) {
					if (dst[j].re != dst[i].re) 
						continue;

					t.x1 += dst[j].x1;
					t.y1 += dst[j].y1;
					t.x2 += dst[j].x2;
					t.y2 += dst[j].y2;
					t.count = dst[j].count;
					dst[j].count = -1;
				}

				t.x1 = t.x1 / t.count;
				t.y1 = t.y1 / t.count;
				t.x2 = t.x2 / t.count;
				t.y2 = t.y2 / t.count;

				dets.Push(t);

				if (t.count <= 1) 
					printf("ERROR: COUNT WRONG!");
			}
		}
	}

	return true;
}


void ImageDetector::DrawDetection(cv::Mat &img) {

	for (int i = 0; i < dets.size; i++) {
		int flag = 0;

		cv::Point pt1, pt2;
		pt1.x = dets[i].x1;
		pt1.y = dets[i].y1;
		pt2.x = dets[i].x2;
		pt2.y = dets[i].y2;
		

		for (int j = 0; j < dets.size; j++) {

			if (i != j &&
				dets[i].x1 >= dets[j].x1 && dets[i].x2 <= dets[j].x2 &&
				dets[i].y1 >= dets[j].y1 && dets[i].y2 <= dets[j].y2 &&
				dets[j].re != 110)
			{
				flag = 1;
				dets[i].re = 110;
			}
			else if (i != j && IsOverlap(dets[i], dets[j]) == 1 && dets[j].re != 110 &&
				(dets[i].x2 - dets[i].x1)*(dets[i].y2 - dets[i].y1) <= (dets[j].x2 - dets[j].x1)*(dets[j].y2 - dets[j].y1))
			{
				flag = 1;
				dets[i].re = 110;
			}
		}
		
		if (flag == 0)
			cv::rectangle(img, pt1, pt2, cv::Scalar(0.f, 255.f, 0.f, 1.f), 3);
	}
}


bool ImageDetector::IsEqual(const rect &r1, const rect &r2) const {
	int distance = cvRound((r1.y2 - r1.y1)*0.2);
	int distance2 = cvRound((r2.y2 - r2.y1)*0.2);

	if (r2.x1 <= r1.x1 + distance && r2.x1 >= r1.x1 - distance &&
		r2.y1 <= r1.y1 + distance && r2.y1 >= r1.y1 - distance &&
		(r2.y2 - r2.y1) <= cvRound((r1.y2 - r1.y1) * 1.2) &&
		(r1.y2 - r1.y1) <= cvRound((r2.y2 - r2.y1) * 1.2))
		return true;
	else if (r1.x1 <= r2.x1 + distance2 && r1.x1 >= r2.x1 - distance2 &&
		r1.y1 <= r2.y1 + distance2 && r1.y1 >= r2.y1 - distance2 &&
		(r1.y2 - r1.y1) <= cvRound((r2.y2 - r2.y1) * 1.2) &&
		(r2.y2 - r2.y1) <= cvRound((r1.y2 - r1.y1) * 1.2))
		return true;
	else
		return false;
}

bool ImageDetector::IsOverlap(const rect &r, const rect &t) const {
	int h1, h2;
	int w1, w2;
	h1 = r.x1 < t.x1 ? r.x1 : t.x1;
	h2 = r.x2 > t.x2 ? r.x2 : t.x2;
	w1 = r.y1 < t.y1 ? r.y1 : t.y1;
	w2 = r.y2 > t.y2 ? r.y2 : t.y2;

	double count = 0;
	for (int i = h1; i < h2; i++)
		for (int j = w1; j < w2; j++)
			if (i >= r.x1 && i < r.x2 && j >= r.y1 && j < r.y2
				&& i >= t.x1 && i < t.x2 && j >= t.y1 && j < t.y2)
				count++;

	if (count / (double)((r.x2 - r.x1)*(r.y2 - r.y1)) >= 0.8)
		return 1;
	else
		return 0;
}

void ImageDetector::Clear() {
	dets.clear();
}