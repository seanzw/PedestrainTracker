#include "HogDetection.h"
#include "FeatureExtractor.h"

const AdaBoostClassifier classf("..\\HOGClassifier\\");

int IsEqual(rect *r1, rect *r2)
{
	int distance = cvRound((r1->y2 - r1->y1)*0.2);
	int distance2 = cvRound((r2->y2 - r2->y1)*0.2);

	if (r2->x1 <= r1->x1 + distance && r2->x1 >= r1->x1 - distance &&
		r2->y1 <= r1->y1 + distance && r2->y1 >= r1->y1 - distance &&
		(r2->y2 - r2->y1) <= cvRound((r1->y2 - r1->y1) * 1.2) &&
		(r1->y2 - r1->y1) <= cvRound((r2->y2 - r2->y1) * 1.2))
		return 1;
	else if (r1->x1 <= r2->x1 + distance2 && r1->x1 >= r2->x1 - distance2 &&
		r1->y1 <= r2->y1 + distance2 && r1->y1 >= r2->y1 - distance2 &&
		(r1->y2 - r1->y1) <= cvRound((r2->y2 - r2->y1) * 1.2) &&
		(r2->y2 - r2->y1) <= cvRound((r1->y2 - r1->y1) * 1.2))
		return 1;
	else
		return 0;
}


int IsOverlap(rect *r, rect *t)
{
	int h1, h2;
	int w1, w2;
	h1 = r->x1 < t->x1 ? r->x1 : t->x1;
	h2 = r->x2 > t->x2 ? r->x2 : t->x2;
	w1 = r->y1 < t->y1 ? r->y1 : t->y1;
	w2 = r->y2 > t->y2 ? r->y2 : t->y2;

	double count = 0;
	for (int i = h1; i < h2; i++)
		for (int j = w1; j < w2; j++)
			if (i >= r->x1 && i < r->x2 && j >= r->y1 && j < r->y2
				&& i >= t->x1 && i < t->x2 && j >= t->y1 && j < t->y2)
				count++;

	if (count / (double)((r->x2 - r->x1)*(r->y2 - r->y1)) >= 0.8)
		return 1;
	else
		return 0;
}

void Detection(
	IplImage *img, IplImage *gray,
	double smin, double smax,
	double scalestep, int slidestep, int neighbor)
{
	// Initialize the FeatureExtractor.
	HoGExtractor hogExtractor(gray->width, gray->height);
	hogExtractor.Preprocess(cv::cvarrToMat(gray));

	CvSeq *seq = 0;
	CvMemStorage *storage = cvCreateMemStorage(0);
	seq = cvCreateSeq(0, sizeof(CvSeq), sizeof(rect), storage);

	CvSeq *seq1 = 0;
	CvMemStorage *storage1 = cvCreateMemStorage(0);
	seq1 = cvCreateSeq(0, sizeof(CvSeq), sizeof(rect), storage1);

	for (double scale = smin; scale <= smax; scale = scale*scalestep)
	{
		for (int i = 0; i <= img->height - HEIGHT*scale; i = i + (int)(slidestep*scale))
		{
			for (int j = 0; j <= img->width - WIDTH*scale; j = j + (int)(slidestep*scale))
			{
				bool is = classf.Classify(i, j, scale, hogExtractor);
				// save current window
				if (is)
				{
					rect store;
					store.x1 = j;
					store.y1 = i;
					store.x2 = j + (int)(WIDTH*scale);
					store.y2 = i + (int)(HEIGHT*scale);
					store.re = -1;
					store.count = 1;
					cvSeqPush(seq, &store);
				}
			}
		}
	}

	// merge windows
	if (neighbor > 1)
	{
		for (int i = 0; i < seq->total; i++)
		{
			rect *r1 = (rect*)cvGetSeqElem(seq, i);
			for (int j = i + 1; j < seq->total; j++)
			{
				rect *r2 = (rect*)cvGetSeqElem(seq, j);
				if (IsEqual(r1, r2) == 1) // merge
				{
					if (r1->re == -1) { r1->re = i; r2->re = i; }
					else if (r1->re >= 0) { r2->re = r1->re; }
				}
			}
		}

		for (int i = 0; i < seq->total; i++)
		{
			int count = 0;
			rect *r1 = (rect*)cvGetSeqElem(seq, i);
			for (int j = i; j < seq->total; j++)
			{
				rect *r2 = (rect*)cvGetSeqElem(seq, j);
				if (r1->re == i && r2->re == i) ++count;
			}

			if (r1->re == i) r1->count = count;
			for (int j = i; j < seq->total; j++)
			{
				rect *r2 = (rect*)cvGetSeqElem(seq, j);
				if (r1->re == i && r2->re == i) r2->count = count;
			}
		}
	}

	for (int i = 0; i < seq->total; i++)
	{
		rect r = *(rect*)cvGetSeqElem(seq, i);
		if (r.re == -1) continue;

		if (r.re >= 0 && r.count >= neighbor)
		{
			rect t;
			t.x1 = 0;
			t.y1 = 0;
			t.x2 = 0;
			t.y2 = 0;

			for (int j = i; j < seq->total; j++)
			{
				rect *temp = (rect*)cvGetSeqElem(seq, j);
				if (temp->re != r.re) continue;

				t.x1 += temp->x1;
				t.y1 += temp->y1;
				t.x2 += temp->x2;
				t.y2 += temp->y2;
				t.count = temp->count;
				temp->count = -1;
			}

			t.x1 = t.x1 / t.count;
			t.y1 = t.y1 / t.count;
			t.x2 = t.x2 / t.count;
			t.y2 = t.y2 / t.count;
			cvSeqPush(seq1, &t);
			if (t.count <= 1) cout << "ERROR!" << endl;
		}
	}

	// draw result
	for (int i = 0; i < (seq1 ? seq1->total : 0); i++)
	{
		int flag = 0;
		rect *r = (rect*)cvGetSeqElem(seq1, i);

		CvPoint pt1, pt2;
		pt1.x = r->x1;
		pt1.y = r->y1;
		pt2.x = r->x2;
		pt2.y = r->y2;

		for (int j = 0; j < (seq1 ? seq1->total : 0); j++)
		{
			rect *t = (rect*)cvGetSeqElem(seq1, j);

			if (i != j &&
				r->x1 >= t->x1 && r->x2 <= t->x2 &&
				r->y1 >= t->y1 && r->y2 <= t->y2 &&
				t->re != 110)
			{
				flag = 1;
				r->re = 110;
			}
			else if (i != j && IsOverlap(r, t) == 1 && t->re != 110 &&
				(r->x2 - r->x1)*(r->y2 - r->y1) <= (t->x2 - t->x1)*(t->y2 - t->y1))
			{
				flag = 1;
				r->re = 110;
			}
		}

		if (flag == 0) cvRectangle(img, pt1, pt2, CV_RGB(0, 255, 0), 2, 8, 0);
	}

	cvReleaseMemStorage(&storage);
	cvReleaseMemStorage(&storage1);
}

void Detection(
	IplImage *img, IplImage *gray, CvRect roi_cur,
	CvSeq* &rect_seq,
	double smin, double smax,
	double scalestep, int slidestep, int neighbor)
{
	// Initialize the FeatureExtractor.
	HoGExtractor hogExtractor(gray->width, gray->height);
	hogExtractor.Preprocess(cv::cvarrToMat(gray));

	CvSeq *seq = 0;
	CvMemStorage *storage = cvCreateMemStorage(0);
	seq = cvCreateSeq(0, sizeof(CvSeq), sizeof(rect), storage);

	CvSeq *seq1 = 0;
	CvMemStorage *storage1 = cvCreateMemStorage(0);
	seq1 = cvCreateSeq(0, sizeof(CvSeq), sizeof(rect), storage1);

	for (double scale = smin; scale <= smax; scale = scale*scalestep)
	{
		for (int i = 0; i <= img->height - HEIGHT*scale; i = i + (int)(slidestep*scale))
		{
			for (int j = 0; j <= img->width - WIDTH*scale; j = j + (int)(slidestep*scale))
			{
				bool is = classf.Classify(i, j, scale, hogExtractor);
				// save current window
				if (is)
				{
					rect store;
					store.x1 = j;
					store.y1 = i;
					store.x2 = j + (int)(WIDTH*scale);
					store.y2 = i + (int)(HEIGHT*scale);
					store.re = -1;
					store.count = 1;
					cvSeqPush(seq, &store);
				}
			}
		}
	}

	// merge windows
	if (neighbor > 1)
	{
		for (int i = 0; i < seq->total; i++)
		{
			rect *r1 = (rect*)cvGetSeqElem(seq, i);
			for (int j = i + 1; j < seq->total; j++)
			{
				rect *r2 = (rect*)cvGetSeqElem(seq, j);
				if (IsEqual(r1, r2) == 1) // merge
				{
					if (r1->re == -1) { r1->re = i; r2->re = i; }
					else if (r1->re >= 0) { r2->re = r1->re; }
				}
			}
		}

		for (int i = 0; i < seq->total; i++)
		{
			int count = 0;
			rect *r1 = (rect*)cvGetSeqElem(seq, i);
			for (int j = i; j < seq->total; j++)
			{
				rect *r2 = (rect*)cvGetSeqElem(seq, j);
				if (r1->re == i && r2->re == i) ++count;
			}

			if (r1->re == i) r1->count = count;
			for (int j = i; j < seq->total; j++)
			{
				rect *r2 = (rect*)cvGetSeqElem(seq, j);
				if (r1->re == i && r2->re == i) r2->count = count;
			}
		}
	}

	for (int i = 0; i < seq->total; i++)
	{
		rect r = *(rect*)cvGetSeqElem(seq, i);
		if (r.re == -1) continue;

		if (r.re >= 0 && r.count >= neighbor)
		{
			rect t;
			t.x1 = 0;
			t.y1 = 0;
			t.x2 = 0;
			t.y2 = 0;

			for (int j = i; j < seq->total; j++)
			{
				rect *temp = (rect*)cvGetSeqElem(seq, j);
				if (temp->re != r.re) continue;

				t.x1 += temp->x1;
				t.y1 += temp->y1;
				t.x2 += temp->x2;
				t.y2 += temp->y2;
				t.count = temp->count;
				temp->count = -1;
			}

			t.x1 = t.x1 / t.count;
			t.y1 = t.y1 / t.count;
			t.x2 = t.x2 / t.count;
			t.y2 = t.y2 / t.count;
			cvSeqPush(seq1, &t);
			if (t.count <= 1) cout << "ERROR!" << endl;
		}
	}

	// draw result
	for (int i = 0; i < (seq1 ? seq1->total : 0); i++)
	{
		int flag = 0;
		rect *r = (rect*)cvGetSeqElem(seq1, i);

		CvPoint pt1, pt2;
		pt1.x = r->x1;
		pt1.y = r->y1;
		pt2.x = r->x2;
		pt2.y = r->y2;

		for (int j = 0; j < (seq1 ? seq1->total : 0); j++)
		{
			rect *t = (rect*)cvGetSeqElem(seq1, j);

			if (i != j &&
				r->x1 >= t->x1 && r->x2 <= t->x2 &&
				r->y1 >= t->y1 && r->y2 <= t->y2 &&
				t->re != 110)
			{
				flag = 1;
				r->re = 110;
			}
			else if (i != j && IsOverlap(r, t) == 1 && t->re != 110 &&
				(r->x2 - r->x1)*(r->y2 - r->y1) <= (t->x2 - t->x1)*(t->y2 - t->y1))
			{
				flag = 1;
				r->re = 110;
			}
		}

		if (flag == 0) //cvRectangle(img,pt1,pt2,CV_RGB(0,255,0),2,8,0);
		{
			CvRect r1;
			r1.x = min(pt1.x, pt2.x) + roi_cur.x;
			r1.y = min(pt1.y, pt2.y) + roi_cur.y;
			r1.width = abs(pt1.x - pt2.x + 1);
			r1.height = abs(pt1.y - pt2.y + 1);
			cvSeqPush(rect_seq, &r1);
			//cvRectangle(img,pt1,pt2,CV_RGB(255,0,0),2,8,0);
		}
	}

	//cvRectangle(img,cvPoint(0,0),cvPoint(img->width-1,img->height-1),CV_RGB(255,0,0),1,8,0);

	cvReleaseMemStorage(&storage);
	cvReleaseMemStorage(&storage1);
}

void Detection2(
	IplImage *img, IplImage *gray,
	double smin, double smax,
	double scalestep, int slidestep, int neighbor, CvRect roi)
{
	// Initialize the FeatureExtractor.
	HoGExtractor hogExtractor(gray->width, gray->height);
	hogExtractor.Preprocess(cv::cvarrToMat(gray));

	CvSeq *seq = 0;
	CvMemStorage *storage = cvCreateMemStorage(0);
	seq = cvCreateSeq(0, sizeof(CvSeq), sizeof(rect), storage);

	CvSeq *seq1 = 0;
	CvMemStorage *storage1 = cvCreateMemStorage(0);
	seq1 = cvCreateSeq(0, sizeof(CvSeq), sizeof(rect), storage1);

	for (double scale = smin; scale <= smax; scale = scale*scalestep)
	{
		for (int i = 0; i <= img->height - HEIGHT*scale; i = i + (int)(slidestep*scale))
		{
			for (int j = 0; j <= img->width - WIDTH*scale; j = j + (int)(slidestep*scale))
			{
				bool is = classf.Classify(i, j, scale, hogExtractor);
				// save current window
				if (is)
				{
					rect store;
					store.x1 = j + roi.x;
					store.y1 = i + roi.y;
					store.x2 = j + (int)(WIDTH*scale) + roi.x;
					store.y2 = i + (int)(HEIGHT*scale) + roi.y;
					store.re = -1;
					store.count = 1;

					//cout<<"("<<store.x1<<","<<store.y1<<"), ("<<store.x2<<","<<store.y2<<")"<<endl;

					cvSeqPush(seq, &store);
				}
			}
		}
	}

	// merge windows
	if (neighbor > 1)
	{
		for (int i = 0; i < seq->total; i++)
		{
			rect *r1 = (rect*)cvGetSeqElem(seq, i);
			for (int j = i + 1; j < seq->total; j++)
			{
				rect *r2 = (rect*)cvGetSeqElem(seq, j);
				if (IsEqual(r1, r2) == 1) // merge
				{
					if (r1->re == -1) { r1->re = i; r2->re = i; }
					else if (r1->re >= 0) { r2->re = r1->re; }
				}
			}
		}

		for (int i = 0; i < seq->total; i++)
		{
			int count = 0;
			rect *r1 = (rect*)cvGetSeqElem(seq, i);
			for (int j = i; j < seq->total; j++)
			{
				rect *r2 = (rect*)cvGetSeqElem(seq, j);
				if (r1->re == i && r2->re == i) ++count;
			}

			if (r1->re == i) r1->count = count;
			for (int j = i; j < seq->total; j++)
			{
				rect *r2 = (rect*)cvGetSeqElem(seq, j);
				if (r1->re == i && r2->re == i) r2->count = count;
			}
		}
	}

	for (int i = 0; i < seq->total; i++)
	{
		rect r = *(rect*)cvGetSeqElem(seq, i);
		if (r.re == -1) continue;

		if (r.re >= 0 && r.count >= neighbor)
		{
			rect t;
			t.x1 = 0;
			t.y1 = 0;
			t.x2 = 0;
			t.y2 = 0;

			for (int j = i; j < seq->total; j++)
			{
				rect *temp = (rect*)cvGetSeqElem(seq, j);
				if (temp->re != r.re) continue;

				t.x1 += temp->x1;
				t.y1 += temp->y1;
				t.x2 += temp->x2;
				t.y2 += temp->y2;
				t.count = temp->count;
				temp->count = -1;
			}

			t.x1 = t.x1 / t.count;
			t.y1 = t.y1 / t.count;
			t.x2 = t.x2 / t.count;
			t.y2 = t.y2 / t.count;
			cvSeqPush(seq1, &t);
			if (t.count <= 1) cout << "ERROR!" << endl;
		}
	}

	// draw result
	for (int i = 0; i < (seq1 ? seq1->total : 0); i++)
	{
		int flag = 0;
		rect *r = (rect*)cvGetSeqElem(seq1, i);

		CvPoint pt1, pt2;
		pt1.x = r->x1;
		pt1.y = r->y1;
		pt2.x = r->x2;
		pt2.y = r->y2;

		for (int j = 0; j < (seq1 ? seq1->total : 0); j++)
		{
			rect *t = (rect*)cvGetSeqElem(seq1, j);

			if (i != j &&
				r->x1 >= t->x1 && r->x2 <= t->x2 &&
				r->y1 >= t->y1 && r->y2 <= t->y2 &&
				t->re != 110)
			{
				flag = 1;
				r->re = 110;
			}
			else if (i != j && IsOverlap(r, t) == 1 && t->re != 110 &&
				(r->x2 - r->x1)*(r->y2 - r->y1) <= (t->x2 - t->x1)*(t->y2 - t->y1))
			{
				flag = 1;
				r->re = 110;
			}
		}

		if (flag == 0)
		{
			pt1.x = pt1.x - roi.x; pt1.y = pt1.y - roi.y;
			pt2.x = pt2.x - roi.x; pt2.y = pt2.y - roi.y;
			cvRectangle(img, pt1, pt2, CV_RGB(0, 255, 0), 2, 8, 0);
			//cout<<"("<<pt1.x<<","<<pt1.y<<"), ("<<pt2.x<<","<<pt2.y<<")"<<endl;
		}
	}

	//cvRectangle(img,cvPoint(0,0),cvPoint(img->width-1,img->height-1),CV_RGB(255,0,0),1,8,0);

	cvReleaseMemStorage(&storage);
	cvReleaseMemStorage(&storage1);
}
