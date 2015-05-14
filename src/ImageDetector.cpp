#include "ImageDetector.h"

ImageDetector::ImageDetector(Classifier *c, const Options &op) {
	classifier = c;
	scaleMin = op.scaleMin;
	scaleMax = op.scaleMax;
	scaleStep = op.scaleStep;
	slideStep = op.slideStep;
	evidence = op.evidence;
	modelWidth = op.modelWidth;
	modelHeight = op.modelHeight;
}

bool ImageDetector::Detect(const cv::Mat &img, const IntegralImage *intImage,
	const Rect &subRegion,
	const cv::Mat &bkg
	) {

	// Clear.
	temp.clear();

	Rect roi;
	// Slide the window and detect.
	for (float scale = scaleMin; scale < scaleMax; scale *= scaleStep) {
		roi.width = (int)(scale * modelWidth);
		roi.height = (int)(scale * modelHeight);
		for (int i = 0; i <= subRegion.height - modelHeight * scale; i = i + (int)(slideStep * scale)) {
			for (int j = 0; j <= subRegion.width - modelWidth * scale; j = j + (int)(slideStep * scale)) {
				roi.upper = i + subRegion.upper;
				roi.left = j + subRegion.left;
				if (classifier->Classify(intImage, roi, scale) > 0) {
					temp.Push(rect(roi.left,
						roi.upper,
						roi.left + roi.width,
						roi.upper + roi.height));
				}
			}
		}
	}

	//unionFind = new UnionFind(temp.size);
	//for (int i = 0; i < temp.size; i++) {
	//	for (int j = i + 1; j < temp.size; j++) {
	//		if (IsEqual(temp[i], temp[j])) {
	//			// Union.
	//			unionFind->Union(i, j);
	//		}
	//	}
	//}

	//// Merge them to get the new detection.
	//roots.clear();

	//// Store the original number of detections.
	//int originalDetections = dets.size;

	//for (int i = 0; i < temp.size; i++) {
	//	int root = unionFind->Find(i);
	//	int size = unionFind->GetSize(root);
	//	/*std::cout << size << std::endl;*/

	//	// This component is big enough.
	//	if (size >= evidence) {

	//		// Which components is it in?
	//		int index;

	//		for (index = 0; index < roots.size; index++) {
	//			if (roots[index] == root) {
	//				break;
	//			}
	//		}

	//		if (index == roots.size) {
	//			// We have met a new component.
	//			roots.Push(root);
	//			dets.Push(temp[i]);
	//		}
	//		else {
	//			// Add temp[i] to the component.
	//			dets[index].x1 += temp[i].x1;
	//			dets[index].y1 += temp[i].y1;
	//			dets[index].x2 += temp[i].x2;
	//			dets[index].y2 += temp[i].y2;
	//		}
	//	}
	//}

	//// Normalize the detections.
	//for (int i = originalDetections; i < dets.size; i++) {
	//	int size = unionFind->GetSize(roots[i]);
	//	dets[i].x1 /= size;
	//	dets[i].y1 /= size;
	//	dets[i].x2 /= size;
	//	dets[i].y2 /= size;
	//}


	// Merge the detections.
	if (evidence > 1) {

		// Union.
		for (int i = 0; i < temp.size; i++) {
			for (int j = i; j < temp.size; j++) {
				if (IsEqual(temp[i], temp[j])) {
					if (temp[i].re == -1) {
						temp[i].re = temp[j].re = i;
					}
					else {
						temp[j].re = temp[i].re;
					}
				}
			}
		}

		// Count.
		for (int i = 0; i < temp.size; i++) {
			int count = 0;
			for (int j = i; j < temp.size; j++) {
				if (temp[i].re == i && temp[j].re == i)
					count++;
			}

			if (temp[i].re == i)
				temp[i].count = count;

			for (int j = i; j < temp.size; j++) {
				if (temp[i].re == i && temp[j].re == i)
					temp[j].count = count;
			}
		}
	}

	// Merge them...
	for (int i = 0; i < temp.size; i++)
	{
		// This one has no friends...
		if (temp[i].re == -1)
			continue;

		if (temp[i].re >= 0 && temp[i].count >= evidence) {
			rect t(0, 0, 0, 0);

			for (int j = i; j < temp.size; j++) {
				if (temp[j].re != temp[i].re)
					continue;

				t.x1 += temp[j].x1;
				t.y1 += temp[j].y1;
				t.x2 += temp[j].x2;
				t.y2 += temp[j].y2;
				t.count = temp[j].count;
				temp[j].count = -1;
			}

			t.x1 = t.x1 / t.count;
			t.y1 = t.y1 / t.count;
			t.x2 = t.x2 / t.count;
			t.y2 = t.y2 / t.count;

			dets.Push(Rect(t.y1, t.x1, t.x2 - t.x1, t.y2 - t.y1));

			if (t.count <= 1)
				printf("ERROR: COUNT WRONG!");
		}
	}


	return true;
}


void ImageDetector::DrawDetection(cv::Mat &img) {

	for (int i = 0; i < dets.size; i++) {
		int flag = 0;

		/*cv::Point pt1, pt2;
		pt1.x = dets[i].left;
		pt1.y = dets[i].upper;
		pt2.x = dets[i].x2;
		pt2.y = dets[i].y2;*/


		//for (int j = 0; j < dets.size; j++) {

		//	if (i != j &&
		//		dets[i].x1 >= dets[j].x1 && dets[i].x2 <= dets[j].x2 &&
		//		dets[i].y1 >= dets[j].y1 && dets[i].y2 <= dets[j].y2 &&
		//		dets[j].re != 110)
		//	{
		//		flag = 1;
		//		dets[i].re = 110;
		//	}
		//	else if (i != j && IsOverlap(dets[i], dets[j]) == 1 && dets[j].re != 110 &&
		//		(dets[i].x2 - dets[i].x1)*(dets[i].y2 - dets[i].y1) <= (dets[j].x2 - dets[j].x1)*(dets[j].y2 - dets[j].y1))
		//	{
		//		flag = 1;
		//		dets[i].re = 110;
		//	}
		//}

		if (flag == 0)
			cv::rectangle(img, (cv::Rect)dets[i], cv::Scalar(0.f, 255.f, 0.f, 1.f), 2);
			//cv::rectangle(img, pt1, pt2, cv::Scalar(0.f, 255.f, 0.f, 1.f), 2);
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