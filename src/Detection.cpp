#include "HogDetection.h"
#include "HogTracking.h"
#include "FeatureExtractor.hpp"

#define SINPI1 0.173648
#define COSPI1 0.984808
#define SINPI2 0.500000
#define COSPI2 0.866025
#define SINPI3 0.766044
#define COSPI3 0.642788
#define SINPI4 0.939693
#define COSPI4 0.342020
#define SINPI5 1.000000
#define COSPI5 0.000000
#define SINPI6 0.939693
#define COSPI6 -0.342020
#define SINPI7 0.766044
#define COSPI7 -0.642788
#define SINPI8 0.500000
#define COSPI8 -0.866025
#define SINPI9 0.173648
#define COSPI9 -0.984808

const double xVector[9] = { COSPI1, COSPI2, COSPI3, COSPI4, COSPI5, COSPI6, COSPI7, COSPI8, COSPI9 };
const double yVector[9] = { SINPI1, SINPI2, SINPI3, SINPI4, SINPI5, SINPI6, SINPI7, SINPI8, SINPI9 };

inline int Direct(double x, double y) {
	double max = 0.0;
	int ret = 0;
	for (int i = 0; i < 9; i++) {
		double proj = fabs(x * xVector[i] + y * yVector[i]);
		if (proj > max) {
			max = proj;
			ret = i;
		}
	}
	return ret;
}

int Direct(double x)
{
	if (x >= 0 && x < pi / 9) return 0;
	else if (x >= pi / 9 && x < pi * 2 / 9) return 1;
	else if (x >= pi * 2 / 9 && x < pi * 3 / 9) return 2;
	else if (x >= pi * 3 / 9 && x < pi * 4 / 9) return 3;
	else if (x >= pi * 4 / 9 && x < pi * 5 / 9) return 4;
	else if (x >= pi * 5 / 9 && x < pi * 6 / 9) return 5;
	else if (x >= pi * 6 / 9 && x < pi * 7 / 9) return 6;
	else if (x >= pi * 7 / 9 && x < pi * 8 / 9) return 7;
	else if (x >= pi * 8 / 9 && x < pi * 9 / 9) return 8;
	else { cout << "ERROR!" << endl << "x=" << x << endl; return 0; }
}

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

void cal_HOG(IplImage* img, integral **source, integral **s)
{

	for (int i = 0; i < 1; i++)
	{
		for (int j = 0; j < img->width; j++)
		{
			for (int k = 0; k < 9; k++)
			{
				source[i][j].direction[k] = 0;
				s[i][j].direction[k] = 0;
			}
		}
	}

	// calculate HOG
	int height = img->height;
	int width = img->width;
	int step = img->widthStep;
	uchar *data = (uchar *)img->imageData;

	for (int i = 1; i < height - 1; i++)
	{
		for (int j = 1; j < width - 1; j++)
		{
			double tempX = (double)data[(i - 1)*step + (j + 1)] - (double)data[(i - 1)*step + (j - 1)]
				+ 2 * (double)data[(i)*step + (j + 1)] - 2 * (double)data[(i)*step + (j - 1)]
				+ (double)data[(i + 1)*step + (j + 1)] - (double)data[(i + 1)*step + (j - 1)];

			double tempY = (double)data[(i + 1)*step + (j - 1)] - (double)data[(i - 1)*step + (j - 1)]
				+ 2 * (double)data[(i + 1)*step + (j)] - 2 * (double)data[(i - 1)*step + (j)]
				+ (double)data[(i + 1)*step + (j + 1)] - (double)data[(i - 1)*step + (j + 1)];

			double x = atan2(tempY, tempX);
			double temp = sqrt(tempX*tempX + tempY*tempY);

			if (x < 0) x += pi;
			else if (x > pi) x -= pi;

			if (i == 1)
			{
				s[i - 1][j - 1].direction[Direct(x)] = temp;
			}
			else
			{
				s[i - 1][j - 1].direction[0] = s[i - 1 - 1][j - 1].direction[0];
				s[i - 1][j - 1].direction[1] = s[i - 1 - 1][j - 1].direction[1];
				s[i - 1][j - 1].direction[2] = s[i - 1 - 1][j - 1].direction[2];
				s[i - 1][j - 1].direction[3] = s[i - 1 - 1][j - 1].direction[3];
				s[i - 1][j - 1].direction[4] = s[i - 1 - 1][j - 1].direction[4];
				s[i - 1][j - 1].direction[5] = s[i - 1 - 1][j - 1].direction[5];
				s[i - 1][j - 1].direction[6] = s[i - 1 - 1][j - 1].direction[6];
				s[i - 1][j - 1].direction[7] = s[i - 1 - 1][j - 1].direction[7];
				s[i - 1][j - 1].direction[8] = s[i - 1 - 1][j - 1].direction[8];

				s[i - 1][j - 1].direction[Direct(x)] = s[i - 1 - 1][j - 1].direction[Direct(x)] + temp;
			}

			if (j == 1)
			{
				source[i - 1][j - 1].direction[0] = s[i - 1][j - 1].direction[0];
				source[i - 1][j - 1].direction[1] = s[i - 1][j - 1].direction[1];
				source[i - 1][j - 1].direction[2] = s[i - 1][j - 1].direction[2];
				source[i - 1][j - 1].direction[3] = s[i - 1][j - 1].direction[3];
				source[i - 1][j - 1].direction[4] = s[i - 1][j - 1].direction[4];
				source[i - 1][j - 1].direction[5] = s[i - 1][j - 1].direction[5];
				source[i - 1][j - 1].direction[6] = s[i - 1][j - 1].direction[6];
				source[i - 1][j - 1].direction[7] = s[i - 1][j - 1].direction[7];
				source[i - 1][j - 1].direction[8] = s[i - 1][j - 1].direction[8];
			}
			else
			{
				source[i - 1][j - 1].direction[0] = source[i - 1][j - 1 - 1].direction[0] + s[i - 1][j - 1].direction[0];
				source[i - 1][j - 1].direction[1] = source[i - 1][j - 1 - 1].direction[1] + s[i - 1][j - 1].direction[1];
				source[i - 1][j - 1].direction[2] = source[i - 1][j - 1 - 1].direction[2] + s[i - 1][j - 1].direction[2];
				source[i - 1][j - 1].direction[3] = source[i - 1][j - 1 - 1].direction[3] + s[i - 1][j - 1].direction[3];
				source[i - 1][j - 1].direction[4] = source[i - 1][j - 1 - 1].direction[4] + s[i - 1][j - 1].direction[4];
				source[i - 1][j - 1].direction[5] = source[i - 1][j - 1 - 1].direction[5] + s[i - 1][j - 1].direction[5];
				source[i - 1][j - 1].direction[6] = source[i - 1][j - 1 - 1].direction[6] + s[i - 1][j - 1].direction[6];
				source[i - 1][j - 1].direction[7] = source[i - 1][j - 1 - 1].direction[7] + s[i - 1][j - 1].direction[7];
				source[i - 1][j - 1].direction[8] = source[i - 1][j - 1 - 1].direction[8] + s[i - 1][j - 1].direction[8];
			}
		}
	}
};

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

int District(double value, double min, double max)
{
	if (value <= min) return 0;
	if (value >= max) return 99;

	for (int i = 0; i < 100; i++)
		if (value >= min + (max - min) / 100 * i - 0.000001 && value <= min + (max - min) / 100 * (i + 1) + 0.000001)
			return i;

	cout << "ERROR!" << endl;
	cout << "value:" << value << endl;
	cout << "min:" << min << endl;
	cout << "max:" << max << endl;
	exit(0);
	return -1;
}

void Detection(
	IplImage *img, IplImage *gray,
	feature *classifier[], int numweak[],
	integral **source, integral **s,
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
				int flag = 1;
				int stage = 0;
				for (stage = 0; stage < TOTAL_STAGE; stage++)
				{
					double thre = 0;
					for (int m = 0; m < numweak[stage]; m++)
					{
						
						int ii = i + (int)(classifier[stage][m].h*scale) - 1;
						int jj = j + (int)(classifier[stage][m].w*scale) - 1;
						int scaleWW = (int)(classifier[stage][m].width*scale);
						int scaleHH = (int)(classifier[stage][m].height*scale);

						HoGFeature feature;
						hogExtractor.Extract(scaleWW, scaleHH, ii, jj, &feature);
						double *temValue = feature.hogs;

						// normalize
						double temp = 0;
						for (int k = 0; k < 36; k++) temp += temValue[k];
						if (fabs(temp) / scale < 100) flag = 0;
						else for (int k = 0; k < 36; k++) temValue[k] /= temp;

						// project
						double projectValue = 0;
						for (int k = 0; k < 36; k++)
							projectValue += classifier[stage][m].projection[k] * temValue[k];
						int vector_district = District(projectValue, classifier[stage][m].min, classifier[stage][m].max);
						thre = thre + classifier[stage][m].histogram[vector_district]; // sum of each weak classifier
					}

					if (flag != 1 || thre < classifier[stage][numweak[stage] - 1].threshold) break;
				}

				// save current window
				if (stage >= TOTAL_STAGE)
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

	//cvRectangle(img,cvPoint(0,0),cvPoint(img->width-1,img->height-1),CV_RGB(255,0,0),1,8,0);

	cvReleaseMemStorage(&storage);
	cvReleaseMemStorage(&storage1);
}

void Detection(
	IplImage *img, IplImage *gray, CvRect roi_cur,
	feature *classifier[], int numweak[],
	integral **source, integral **s, CvSeq* &rect_seq,
	double smin, double smax,
	double scalestep, int slidestep, int neighbor)
{
	// initialize integral image
	for (int i = 0; i < 1; i++)
	{
		for (int j = 0; j < img->width; j++)
		{
			for (int k = 0; k < 9; k++)
			{
				source[i][j].direction[k] = 0;
				s[i][j].direction[k] = 0;
			}
		}
	}

	// calculate HOG
	int height = gray->height;
	int width = gray->width;
	int step = gray->widthStep;
	uchar *data = (uchar *)gray->imageData;

	for (int i = 1; i < height - 1; i++)
	{
		for (int j = 1; j < width - 1; j++)
		{
			double tempX = (double)data[(i - 1)*step + (j + 1)] - (double)data[(i - 1)*step + (j - 1)]
				+ 2 * (double)data[(i)*step + (j + 1)] - 2 * (double)data[(i)*step + (j - 1)]
				+ (double)data[(i + 1)*step + (j + 1)] - (double)data[(i + 1)*step + (j - 1)];

			double tempY = (double)data[(i + 1)*step + (j - 1)] - (double)data[(i - 1)*step + (j - 1)]
				+ 2 * (double)data[(i + 1)*step + (j)] - 2 * (double)data[(i - 1)*step + (j)]
				+ (double)data[(i + 1)*step + (j + 1)] - (double)data[(i - 1)*step + (j + 1)];

			//double x = atan2(tempY, tempX);
			int direction = Direct(tempX, tempY);
			double temp = sqrt(tempX*tempX + tempY*tempY);

			//if (x < 0) x += pi;
			//else if (x > pi) x -= pi;

			if (i == 1)
			{
				s[i - 1][j - 1].direction[direction] = temp;
			}
			else
			{
				s[i - 1][j - 1].direction[0] = s[i - 1 - 1][j - 1].direction[0];
				s[i - 1][j - 1].direction[1] = s[i - 1 - 1][j - 1].direction[1];
				s[i - 1][j - 1].direction[2] = s[i - 1 - 1][j - 1].direction[2];
				s[i - 1][j - 1].direction[3] = s[i - 1 - 1][j - 1].direction[3];
				s[i - 1][j - 1].direction[4] = s[i - 1 - 1][j - 1].direction[4];
				s[i - 1][j - 1].direction[5] = s[i - 1 - 1][j - 1].direction[5];
				s[i - 1][j - 1].direction[6] = s[i - 1 - 1][j - 1].direction[6];
				s[i - 1][j - 1].direction[7] = s[i - 1 - 1][j - 1].direction[7];
				s[i - 1][j - 1].direction[8] = s[i - 1 - 1][j - 1].direction[8];

				s[i - 1][j - 1].direction[direction] = s[i - 1 - 1][j - 1].direction[direction] + temp;
			}

			if (j == 1)
			{
				source[i - 1][j - 1].direction[0] = s[i - 1][j - 1].direction[0];
				source[i - 1][j - 1].direction[1] = s[i - 1][j - 1].direction[1];
				source[i - 1][j - 1].direction[2] = s[i - 1][j - 1].direction[2];
				source[i - 1][j - 1].direction[3] = s[i - 1][j - 1].direction[3];
				source[i - 1][j - 1].direction[4] = s[i - 1][j - 1].direction[4];
				source[i - 1][j - 1].direction[5] = s[i - 1][j - 1].direction[5];
				source[i - 1][j - 1].direction[6] = s[i - 1][j - 1].direction[6];
				source[i - 1][j - 1].direction[7] = s[i - 1][j - 1].direction[7];
				source[i - 1][j - 1].direction[8] = s[i - 1][j - 1].direction[8];
			}
			else
			{
				source[i - 1][j - 1].direction[0] = source[i - 1][j - 1 - 1].direction[0] + s[i - 1][j - 1].direction[0];
				source[i - 1][j - 1].direction[1] = source[i - 1][j - 1 - 1].direction[1] + s[i - 1][j - 1].direction[1];
				source[i - 1][j - 1].direction[2] = source[i - 1][j - 1 - 1].direction[2] + s[i - 1][j - 1].direction[2];
				source[i - 1][j - 1].direction[3] = source[i - 1][j - 1 - 1].direction[3] + s[i - 1][j - 1].direction[3];
				source[i - 1][j - 1].direction[4] = source[i - 1][j - 1 - 1].direction[4] + s[i - 1][j - 1].direction[4];
				source[i - 1][j - 1].direction[5] = source[i - 1][j - 1 - 1].direction[5] + s[i - 1][j - 1].direction[5];
				source[i - 1][j - 1].direction[6] = source[i - 1][j - 1 - 1].direction[6] + s[i - 1][j - 1].direction[6];
				source[i - 1][j - 1].direction[7] = source[i - 1][j - 1 - 1].direction[7] + s[i - 1][j - 1].direction[7];
				source[i - 1][j - 1].direction[8] = source[i - 1][j - 1 - 1].direction[8] + s[i - 1][j - 1].direction[8];
			}
		}
	}

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
				int flag = 1;
				int stage = 0;
				for (stage = 0; stage < TOTAL_STAGE; stage++)
				{
					double thre = 0;
					for (int m = 0; m < numweak[stage]; m++)
					{
						int ii = i + (int)(classifier[stage][m].h*scale) - 1;
						int jj = j + (int)(classifier[stage][m].w*scale) - 1;
						int scaleWW = (int)(classifier[stage][m].width*scale);
						int scaleHH = (int)(classifier[stage][m].height*scale);

						double temValue[36];
						for (int k = 0; k < 9; k++)
							temValue[k]
							= (source[ii + (int)(scaleHH / 2.0) - 1][jj + (int)(scaleWW / 2.0) - 1].direction[k]
							+ source[ii - 1][jj - 1].direction[k]
							- source[ii - 1][jj + (int)(scaleWW / 2.0) - 1].direction[k]
							- source[ii + (int)(scaleHH / 2.0) - 1][jj - 1].direction[k]) / (scale*scale);

						for (int k = 0; k < 9; k++)
							temValue[9 + k]
							= (source[ii + (int)(scaleHH / 2.0) - 1][jj + scaleWW - 1].direction[k]
							+ source[ii - 1][jj + (int)(scaleWW / 2.0) - 1].direction[k]
							- source[ii - 1][jj + scaleWW - 1].direction[k]
							- source[ii + (int)(scaleHH / 2.0) - 1][jj + (int)(scaleWW / 2.0) - 1].direction[k]) / (scale*scale);

						for (int k = 0; k < 9; k++)
							temValue[18 + k]
							= (source[ii + scaleHH - 1][jj + (int)(scaleWW / 2.0) - 1].direction[k]
							+ source[ii + (int)(scaleHH / 2.0) - 1][jj - 1].direction[k]
							- source[ii + (int)(scaleHH / 2.0) - 1][jj + (int)(scaleWW / 2.0) - 1].direction[k]
							- source[ii + scaleHH - 1][jj - 1].direction[k]) / (scale*scale);

						for (int k = 0; k < 9; k++)
							temValue[27 + k]
							= (source[ii + scaleHH - 1][jj + scaleWW - 1].direction[k]
							+ source[ii + (int)(scaleHH / 2.0) - 1][jj + (int)(scaleWW / 2.0) - 1].direction[k]
							- source[ii + (int)(scaleHH / 2.0) - 1][jj + scaleWW - 1].direction[k]
							- source[ii + scaleHH - 1][jj + (int)(scaleWW / 2.0) - 1].direction[k]) / (scale*scale);

						// normalize
						double temp = 0;
						for (int k = 0; k < 36; k++) temp += temValue[k];
						if (fabs(temp) / scale < 100) flag = 0;
						else for (int k = 0; k < 36; k++) temValue[k] /= temp;

						// project
						double projectValue = 0;
						for (int k = 0; k < 36; k++)
							projectValue += classifier[stage][m].projection[k] * temValue[k];
						int vector_district = District(projectValue, classifier[stage][m].min, classifier[stage][m].max);
						thre = thre + classifier[stage][m].histogram[vector_district]; // sum of each weak classifier
					}

					if (flag != 1 || thre < classifier[stage][numweak[stage] - 1].threshold) break;
				}

				// save current window
				if (stage >= TOTAL_STAGE)
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
	feature *classifier[], int numweak[],
	integral **source, integral **s,
	double smin, double smax,
	double scalestep, int slidestep, int neighbor, CvRect roi)
{
	//cal_HOG(height,width,data,source,s);

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
				int flag = 1;
				int stage = 0;
				for (stage = 0; stage < TOTAL_STAGE; stage++)
				{
					double thre = 0;
					for (int m = 0; m < numweak[stage]; m++)
					{
						int ii = i + (int)(classifier[stage][m].h*scale) - 1 + roi.y;
						int jj = j + (int)(classifier[stage][m].w*scale) - 1 + roi.x;
						int scaleWW = (int)(classifier[stage][m].width*scale);
						int scaleHH = (int)(classifier[stage][m].height*scale);

						double temValue[36];
						for (int k = 0; k < 9; k++)
							temValue[k]
							= (source[ii + (int)(scaleHH / 2.0) - 1][jj + (int)(scaleWW / 2.0) - 1].direction[k]
							+ source[ii - 1][jj - 1].direction[k]
							- source[ii - 1][jj + (int)(scaleWW / 2.0) - 1].direction[k]
							- source[ii + (int)(scaleHH / 2.0) - 1][jj - 1].direction[k]) / (scale*scale);

						for (int k = 0; k < 9; k++)
							temValue[9 + k]
							= (source[ii + (int)(scaleHH / 2.0) - 1][jj + scaleWW - 1].direction[k]
							+ source[ii - 1][jj + (int)(scaleWW / 2.0) - 1].direction[k]
							- source[ii - 1][jj + scaleWW - 1].direction[k]
							- source[ii + (int)(scaleHH / 2.0) - 1][jj + (int)(scaleWW / 2.0) - 1].direction[k]) / (scale*scale);

						for (int k = 0; k < 9; k++)
							temValue[18 + k]
							= (source[ii + scaleHH - 1][jj + (int)(scaleWW / 2.0) - 1].direction[k]
							+ source[ii + (int)(scaleHH / 2.0) - 1][jj - 1].direction[k]
							- source[ii + (int)(scaleHH / 2.0) - 1][jj + (int)(scaleWW / 2.0) - 1].direction[k]
							- source[ii + scaleHH - 1][jj - 1].direction[k]) / (scale*scale);

						for (int k = 0; k < 9; k++)
							temValue[27 + k]
							= (source[ii + scaleHH - 1][jj + scaleWW - 1].direction[k]
							+ source[ii + (int)(scaleHH / 2.0) - 1][jj + (int)(scaleWW / 2.0) - 1].direction[k]
							- source[ii + (int)(scaleHH / 2.0) - 1][jj + scaleWW - 1].direction[k]
							- source[ii + scaleHH - 1][jj + (int)(scaleWW / 2.0) - 1].direction[k]) / (scale*scale);

						// normalize
						double temp = 0;
						for (int k = 0; k < 36; k++) temp += temValue[k];
						if (fabs(temp) / scale < 100) flag = 0;
						else for (int k = 0; k < 36; k++) temValue[k] /= temp;

						// project
						double projectValue = 0;
						for (int k = 0; k < 36; k++)
							projectValue += classifier[stage][m].projection[k] * temValue[k];
						int vector_district = District(projectValue, classifier[stage][m].min, classifier[stage][m].max);
						thre = thre + classifier[stage][m].histogram[vector_district]; // sum of each weak classifier
					}

					if (flag != 1 || thre < classifier[stage][numweak[stage] - 1].threshold) break;
				}

				// save current window
				if (stage >= TOTAL_STAGE)
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
