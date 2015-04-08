#include "HogTracking.h"
#include <fstream>

using namespace std;

void ReadPolygon(const char *pPolygon, model &md)
{
	char buffer[128];
	//cout<<pPolygon<<endl;
	FILE *pFile;
	fopen_s(&pFile, pPolygon, "r");

	fgets(buffer, 127, pFile); // ImageWidth
	fgets(buffer, 127, pFile); // ImageHeight
	fgets(buffer, 127, pFile); // PolygonPointCount
	cout << buffer << endl;
	int count = atoi(strrchr(buffer, '=') + 1);
	//cout<<count<<endl;

	// read polygon points
	int x, y;
	CvScalar pt;
	CvPoint ding1, ding2;
	md.polygon = cvCreateMat(count, 1, CV_32FC2);

	fgets(buffer, 31, pFile);
	sscanf_s(buffer, "(%d,%d)", &x, &y);
	ding1.x = ding2.x = x;
	ding1.y = ding2.y = y;
	pt.val[0] = (double)x;
	pt.val[1] = (double)y;
	pt.val[2] = 0;
	pt.val[3] = 0;
	cvSet1D(md.polygon, 0, pt);

	for (int k = 1; k < count; ++k)
	{
		fgets(buffer, 31, pFile);
		sscanf_s(buffer, "(%d,%d)", &x, &y);
		ding1.x = (ding1.x < x) ? ding1.x : x;
		ding1.y = (ding1.y < y) ? ding1.y : y;
		ding2.x = (ding2.x > x) ? ding2.x : x;
		ding2.y = (ding2.y > y) ? ding2.y : y;
		pt.val[0] = (double)x;
		pt.val[1] = (double)y;
		pt.val[2] = 0;
		pt.val[3] = 0;
		cvSet1D(md.polygon, k, pt);
	}
	++ding2.x;
	++ding2.y;
	md.roi = cvRect(ding1.x, ding1.y, ding2.x - ding1.x, ding2.y - ding1.y);

	fgets(buffer, 31, pFile);
	sscanf_s(buffer, "(%d,%d)", &md.crosslinept1.x, &md.crosslinept1.y);
	fgets(buffer, 31, pFile);
	sscanf_s(buffer, "(%d,%d)", &md.crosslinept2.x, &md.crosslinept2.y);
	fclose(pFile);
}

CvRect Createroi_t(int x, int y, int roi_h, int roi_w, int edge_width, int height, int width)
{
	int roix, roiy, roih, roiw;
	roix = ((x - edge_width) > 0) ? (x - edge_width) : 1;
	roiy = ((y - edge_width) > 0) ? (y - edge_width) : 1;
	roih = ((roiy + roi_h + 2 * edge_width) > height) ? (height - roiy) : (roi_h + 2 * edge_width);
	roiw = ((roix + roi_w + 2 * edge_width) > width) ? (width - roix) : (roi_w + 2 * edge_width);
	CvRect roi = cvRect(roix, roiy, roiw, roih);
	return roi;
};

void MinusState(info dot[])
{
	for (int i = 0; i < CAPACITY; i++)
	{
		if (dot[0].point[i].state <= 0) continue;
		for (int j = 1; j < TIMEFRAME; j++)
			for (int k = 0; k < CAPACITY; k++)
				if (dot[j].point[k].state != 0 && dot[0].point[i].seriesnumber == dot[j].point[k].seriesnumber) {
					dot[j].point[k].state_count--; break;
				}
	}
}

void UpdateCorrespondDot(info dot[], int dotnum)
{
	for (int i = 0; i < TIMEFRAME - 1; i++)
		for (int j = 0; j < CAPACITY; j++)
			if (dot[i].point[j].state>0 && dot[TIMEFRAME - 1].point[dotnum].seriesnumber == dot[i].point[j].seriesnumber)
				dot[i].point[j].state_count++;
}

void FindCorrespondDot(info dot[], double objectInfo[3])
{
	for (int i = 0; i < CAPACITY; i++)
	{
		if (dot[TIMEFRAME - 1].point[i].state != 1) break;

		double min = 1000;
		int timeseries = -1;
		int dotseries = -1;

		for (int j = TIMEFRAME - 2; j >= 0; j--)
		{
			for (int k = 0; k < CAPACITY; k++)
			{
				if (dot[j].point[k].state > 0)
				{
					double distance = sqrt((dot[TIMEFRAME - 1].point[i].cen_h - dot[j].point[k].predict_h)
						*(dot[TIMEFRAME - 1].point[i].cen_h - dot[j].point[k].predict_h)
						+ (dot[TIMEFRAME - 1].point[i].cen_w - dot[j].point[k].predict_w)
						*(dot[TIMEFRAME - 1].point[i].cen_w - dot[j].point[k].predict_w));

					if (distance < min)
					{
						min = distance;
						timeseries = j;
						dotseries = k;
					}
				}
			}
		}

		if (min <= MERGEDISTANCE && abs(log(dot[TIMEFRAME - 1].point[i].width / dot[timeseries].point[dotseries].width)) <= 0.2)
		{
			dot[TIMEFRAME - 1].point[i].predict_h = dot[TIMEFRAME - 1].point[i].cen_h;
			dot[TIMEFRAME - 1].point[i].predict_w = dot[TIMEFRAME - 1].point[i].cen_w;
			dot[TIMEFRAME - 1].point[i].predict_hv = dot[timeseries].point[dotseries].predict_hv;
			dot[TIMEFRAME - 1].point[i].predict_wv = dot[timeseries].point[dotseries].predict_wv;
			dot[TIMEFRAME - 1].point[i].seriesnumber = dot[timeseries].point[dotseries].seriesnumber;
			dot[TIMEFRAME - 1].point[i].state_count = dot[timeseries].point[dotseries].state_count + 1;
			dot[TIMEFRAME - 1].point[i].state = dot[timeseries].point[dotseries].state;
			dot[TIMEFRAME - 1].point[i].tracknumber = dot[timeseries].point[dotseries].tracknumber;
			UpdateCorrespondDot(dot, i);
		}
		else if (min <= MERGEDISTANCE)
		{
			dot[TIMEFRAME - 1].point[i].state = 0;
		}
		else
		{
			dot[TIMEFRAME - 1].point[i].seriesnumber = (int)objectInfo[0];
			dot[TIMEFRAME - 1].point[i].state_count = 1;
			dot[TIMEFRAME - 1].point[i].predict_h = dot[TIMEFRAME - 1].point[i].cen_h;
			dot[TIMEFRAME - 1].point[i].predict_w = dot[TIMEFRAME - 1].point[i].cen_w;
			dot[TIMEFRAME - 1].point[i].predict_hv = 0;
			dot[TIMEFRAME - 1].point[i].predict_wv = 0;
			objectInfo[0]++;
		}
	}
}

void UpdateState(info dot[], double objectInfo[3])
{
	for (int i = 0; i < TIMEFRAME; i++)
	{
		for (int j = 0; j < CAPACITY; j++)
		{
			switch (dot[i].point[j].state)
			{
			case 1:
				if (dot[i].point[j].state_count >= FRAMENUMBER)
				{
					dot[i].point[j].state = 2;
					dot[i].point[j].tracknumber = (int)objectInfo[2];

					for (int k = i; k < TIMEFRAME; k++)
					{
						for (int m = 0; m < CAPACITY; m++)
						{
							if (dot[k].point[m].seriesnumber == dot[i].point[j].seriesnumber)
							{
								dot[k].point[m].state = 2;
								dot[k].point[m].tracknumber = (int)objectInfo[2];
								if (k == TIMEFRAME - 1) dot[k].point[m].flagcircle = 1;
							}
						}
					}

					objectInfo[2]++;
				}
				break;
			case 2:
				if (dot[i].point[j].state_count < FRAMENUMBER)
					dot[i].point[j].state = 3;
				break;
			case 3:
				if (dot[i].point[j].state_count >= FRAMENUMBER)
					dot[i].point[j].state = 2;
				break;
			default:;
			}
		}
	}
}

void JudgeState(model &md)
{
	MinusState(md.dot);
	for (int i = 0; i < TIMEFRAME - 1; i++) md.dot[i] = md.dot[i + 1];
	md.dot[TIMEFRAME - 1] = md.temdot;
	FindCorrespondDot(md.dot, md.objectInfo);
	UpdateState(md.dot, md.objectInfo);
}

void DrawTarget(IplImage *img, model &md, output &result)
{
	result.num = 0;
	for (int i = (int)md.objectInfo[1]; i < md.objectInfo[2]; i++)
	{
		int start, startk;
		for (start = 0; start < TIMEFRAME; start++)
		{
			for (startk = 0; startk < CAPACITY; startk++)
				if ((md.dot[start].point[startk].state == 2 || md.dot[start].point[startk].state == 3)
					&& md.dot[start].point[startk].tracknumber == i) break;
			if (startk != CAPACITY) break;
		}

		int end, endk;
		for (end = TIMEFRAME - 1; end >= 0; end--)
		{
			for (endk = 0; endk < CAPACITY; endk++)
				if ((md.dot[end].point[endk].state == 2 || md.dot[end].point[endk].state == 3)
					&& md.dot[end].point[endk].tracknumber == i) break;
			if (endk != CAPACITY) break;
		}

		float xx, yy;
		if (start != end)
		{
			xx = (float)md.dot[start].point[startk].cen_w + md.roi.x;
			yy = (float)md.dot[start].point[startk].cen_h + md.roi.y;
			int startside = ((xx - md.crosslinept1.x)*(md.crosslinept2.y - md.crosslinept1.y)
				< (yy - md.crosslinept1.y)*(md.crosslinept2.x - md.crosslinept1.x)) ? 1 : -1;

			xx = (float)md.dot[end].point[endk].cen_w + md.roi.x;
			yy = (float)md.dot[end].point[endk].cen_h + md.roi.y;
			int endside = ((xx - md.crosslinept1.x)*(md.crosslinept2.y - md.crosslinept1.y)
				< (yy - md.crosslinept1.y)*(md.crosslinept2.x - md.crosslinept1.x)) ? 1 : -1;

			if (startside < 0 && endside>0 && md.front.count(i) == 0 && md.back.count(i) == 0) md.front.insert(i);
			if (startside > 0 && endside < 0 && md.front.count(i) == 0 && md.back.count(i) == 0) md.back.insert(i);
		}

		if (end == TIMEFRAME - 1)
		{
			xx = (float)md.dot[end].point[endk].cen_w + md.roi.x;
			yy = (float)md.dot[end].point[endk].cen_h + md.roi.y;
			if ((xx - md.crosslinept1.x)*(md.crosslinept2.y - md.crosslinept1.y)
				< (yy - md.crosslinept1.y)*(md.crosslinept2.x - md.crosslinept1.x))
			{
				part p = md.dot[end].point[endk];
				cvRectangle(img, cvPoint(p.cen_w - p.width / 2, p.cen_h - p.height / 2), cvPoint(p.cen_w + p.width / 2, p.cen_h + p.height / 2), CV_RGB(0, 255, 0), 2, 8, 0);
			}
			else
			{
				part p = md.dot[end].point[endk];
				cvRectangle(img, cvPoint(p.cen_w - p.width / 2, p.cen_h - p.height / 2), cvPoint(p.cen_w + p.width / 2, p.cen_h + p.height / 2), CV_RGB(0, 0, 255), 2, 8, 0);
			}
		}
		else
		{
			if (i == md.objectInfo[1])
				md.objectInfo[1]++;
		}
	}

	result.front = md.front.size();
	result.back = md.back.size();

	TCHAR text[10];
	_itoa_s(result.front, text, 10);
	cvPutText(img, text, cvPoint(0, 50), &md.font, CV_RGB(255, 0, 0));
	_itoa_s(result.back, text, 10);
	cvPutText(img, text, cvPoint(0, 100), &md.font, CV_RGB(255, 0, 0));
}

void UpdatePrediction(info dot[30])
{
	for (int i = 0; i <= TIMEFRAME - 1; i++)
	{
		for (int ii = 0; ii < CAPACITY; ii++)
		{
			int flag = 1;
			if (dot[i].point[ii].state > 0)
			{
				for (int j = TIMEFRAME - 1; j > i && flag == 1; j--)
				{
					for (int jj = 0; jj < CAPACITY && flag == 1; jj++)
					{
						if (dot[i].point[ii].seriesnumber == dot[j].point[jj].seriesnumber
							&& dot[j].point[jj].state == dot[i].point[ii].state)
						{
							if (dot[j].point[jj].flag != 1)
							{
								double h = dot[j].point[jj].cen_h - dot[i].point[ii].cen_h;
								double w = dot[j].point[jj].cen_w - dot[i].point[ii].cen_w;
								double hv = h / ((j - i)*(1.0));
								double wv = w / ((j - i)*(1.0));

								dot[j].point[jj].predict_hv = hv;
								dot[j].point[jj].predict_wv = wv;
								dot[j].point[jj].predict_h = dot[j].point[jj].cen_h + hv*(TIMEFRAME - j*1.0);
								dot[j].point[jj].predict_w = dot[j].point[jj].cen_w + wv*(TIMEFRAME - j*1.0);

								dot[j].point[jj].flag = 1;
							}
							else if (j < TIMEFRAME - 1)
							{
								dot[j].point[jj].predict_h = dot[j].point[jj].cen_h + dot[j].point[jj].predict_hv*(TIMEFRAME - j*1.0);
								dot[j].point[jj].predict_w = dot[j].point[jj].cen_w + dot[j].point[jj].predict_wv*(TIMEFRAME - j*1.0);
							}

							dot[i].point[ii].predict_hv = dot[j].point[jj].predict_hv;
							dot[i].point[ii].predict_wv = dot[j].point[jj].predict_wv;
							dot[i].point[ii].predict_h = dot[j].point[jj].predict_h;
							dot[i].point[ii].predict_w = dot[j].point[jj].predict_w;

							flag = 0;
						}
					}
				}
			}
		}
	}

	for (int i = TIMEFRAME - 1; i >= 0; i--)
	{
		for (int ii = 0; ii < CAPACITY; ii++)
		{
			int flag = 1;
			if (dot[i].point[ii].state > 0)
			{
				for (int j = 0; j < TIMEFRAME && flag == 1; j++)
				{
					for (int jj = 0; jj < CAPACITY && flag == 1; jj++)
					{
						if (dot[i].point[ii].seriesnumber == dot[j].point[jj].seriesnumber
							&& dot[j].point[jj].state == dot[i].point[ii].state)
						{
							if (dot[j].point[jj].flagcircle == 1 && dot[i].point[ii].flagcircle != 1)
							{
								dot[i].point[ii].flagcircle = 1;
								flag = 0;
							}
						}
					}
				}
			}
		}
	}
};

output DetectionTracking(
	IplImage* frame, IplImage* frame_pre, CvRect roi0,
	feature *classifier[], int numweak[],
	double smin, double smax,
	double scalestep, int slidestep, int neighbor, IplImage* bkg1, model &md)
{
	output result;
	md.temdot = Detection_t(frame, frame_pre, roi0, classifier, numweak, smin, smax, scalestep, slidestep, neighbor, bkg1, md);
	JudgeState(md);
	DrawTarget(frame, md, result);
	UpdatePrediction(md.dot);
	return result;
};

void combo_DetectPicture_t(
	IplImage* frame, IplImage* frame_pre,
	CvSeq* &pedestrians, CvRect roi0,
	feature *classifier[], int numweak[],
	double smin, double smax,
	double scalestep, int slidestep, int neighbor, IplImage* bkg1)
{
	//processing picture
	cvSetImageROI(frame, roi0);
	cvSetImageROI(bkg1, roi0);
	IplImage* img_dst = cvCreateImage(cvSize(frame->roi->width, frame->roi->height), IPL_DEPTH_8U, 3);
	cvSmooth(frame, img_dst, CV_GAUSSIAN, GAUSSIAN_SMOOTH_R, GAUSSIAN_SMOOTH_R, 0, 0);

	// get difference between current pictures and background
	IplImage* img_test = cvCreateImage(cvSize(frame->roi->width, frame->roi->height), IPL_DEPTH_8U, 1);
	cvCvtColor(img_dst, img_test, CV_RGB2GRAY);
	CvMat* img_testMat = cvCreateMat(frame->roi->height, frame->roi->width, CV_32FC1);
	cvConvert(img_test, img_testMat);
	CvMat* img_diff1Mat = cvCreateMat(frame->roi->height, frame->roi->width, CV_32FC1);
	CvMat* img_bkg1Mat = cvCreateMat(bkg1->roi->height, bkg1->roi->width, CV_32FC1);
	cvConvert(bkg1, img_bkg1Mat);
	IplImage* img_binary1 = cvCreateImage(cvSize(frame->roi->width, frame->roi->height), IPL_DEPTH_8U, 1);
	cvAbsDiff(img_testMat, img_bkg1Mat, img_diff1Mat);

	// get bwlabels from binary picture
	cvThreshold(img_diff1Mat, img_binary1, THRES_DIFF_BINARY, 255.0, CV_THRESH_BINARY);

	//find candidate Polygons
	int num_Polygon = 0;
	CvSeq* c;
	cv::Mat img_diff = cv::cvarrToMat(img_binary1);
	img_diff = img_diff > 0;
	c = ConnectedComponents(img_diff, 0, THRES_PERIMETER_RATIO, 1, cv::Rect(), cv::Point(-1, -1));

	// if exist candidates of roi
	if (c != NULL)
	{
		for (; c != NULL; ++num_Polygon, c = c->h_next)
		{
			// create roi in each frame
			CvRect roi = cvBoundingRect(c);
			cvResetImageROI(frame);
			if (judge_pedestrian(frame, frame_pre, roi, THRES_AREA_ROI_MAX,
				THRES_AREA_ROI_MIN, THRES_DIFF_FRAME_BINARY, THRES_JUD_RATIO))
			{
				cvSetImageROI(frame, roi0);
				roi = Createroi_t(roi.x, roi.y, roi.height, roi.width, 10, frame->height, frame->width);
				//cvRectangle(frame,cvPoint(roi.x,roi.y),cvPoint(roi.x+roi.width,roi.y+roi.height),CV_RGB(255,0,0),2,8,0);
				cvSeqPush(pedestrians, &roi);
				//cout<<"("<<roi.x<<", "<<roi.y<<"), ("<<roi.x+roi.width<<", "<<roi.y+roi.height<<")"<<endl;
				cout << "Rect " << num_Polygon + 1 << ": Add directly!" << endl;
				//cout<<"Rect "<<num_Polygon+1<<": "<<roi.x<<","<<roi.y<<","<<roi.height<<","<<roi.width<<endl;
			}
			else
			{
				cvSetImageROI(frame, roi0);
				roi = Createroi_t(roi.x, roi.y, roi.height, roi.width, EDGE_WIDTH, frame->height, frame->width);
				cout << "Rect " << num_Polygon + 1 << ": " << roi.x << "," << roi.y << "," << roi.height << "," << roi.width << endl;

				IplImage *img = cvCreateImage(cvSize(roi.width, roi.height), IPL_DEPTH_8U, 3);
				IplImage *gray = cvCreateImage(cvSize(roi.width, roi.height), IPL_DEPTH_8U, 1);

				integral **source = new integral *[roi.height];
				for (int i = 0; i < roi.height; i++)
					source[i] = new integral[roi.width];

				integral **s = new integral *[roi.height];
				for (int i = 0; i < roi.height; i++)
					s[i] = new integral[roi.width];

				cvSetImageROI(frame, roi);
				cvCopy(frame, img);
				cvCvtColor(img, gray, CV_RGB2GRAY);

				Detection(img, gray, roi, classifier, numweak, source, s, pedestrians, smin, smax, scalestep, slidestep, neighbor);

				cout << "Finish Detection Rect " << num_Polygon + 1 << "!" << endl;

				cvCopy(img, frame);
				cvResetImageROI(frame);
				//cvSaveImage(pOut,frame);

				// release integral images
				for (int i = 0; i < roi.height; i++)
				{
					delete[] source[i];
					delete[] s[i];
				}
				delete[] source;
				delete[] s;

				// release data buffer
				cvReleaseImage(&img);
				cvReleaseImage(&gray);
			}
		}
	}

	cvReleaseImage(&img_binary1);
	cvReleaseImage(&img_dst);
	cvReleaseImage(&img_test);
	cvReleaseMat(&img_bkg1Mat);
	cvReleaseMat(&img_diff1Mat);
	cvReleaseMat(&img_testMat);

	cvResetImageROI(frame);
	cvResetImageROI(bkg1);
	return;
}

info Detection_t(
	IplImage* frame, IplImage* frame_pre, CvRect roi0,
	feature *classifier[], int numweak[],
	double smin, double smax,
	double scalestep, int slidestep, int neighbor, IplImage* bkg1, model &md)
{
	info temdot;
	int dotnum = 0;
	CvPoint2D32f pt;
	for (int i = 0; i < CAPACITY; i++)
		temdot.point[i].state = -1;

	CvMemStorage *storage = cvCreateMemStorage(0);

	CvSeq *pedestrians = cvCreateSeq(0, sizeof(CvSeq), sizeof(CvRect), storage);
	combo_DetectPicture_t(frame, frame_pre, pedestrians, roi0, classifier, numweak, smin, smax, scalestep, slidestep, neighbor, bkg1);

	for (int i = 0; i < (pedestrians ? pedestrians->total : 0); i++)
	{
		CvRect *r = (CvRect*)cvGetSeqElem(pedestrians, i);
		//if(r->width>max) continue;
		//曾经的max = 76，min = 38(缺省值)

		CvPoint pt1, pt2;
		pt1.x = r->x, pt1.y = r->y;
		pt2.x = r->x + r->width, pt2.y = r->y + r->height;

		//cout<<"("<<pt1.x<<", "<<pt1.y<<"), ("<<pt2.x<<", "<<pt2.y<<")"<<endl;
		//cvRectangle(frame,pt1,pt2,CV_RGB(255,0,255),2,8,0);

		pt.x = (pt1.x + pt2.x) / 2.0 + md.roi.x, pt.y = (pt1.y + pt2.y) / 2.0 + md.roi.y;
		if (cvPointPolygonTest(md.polygon, pt, 0) > 0)
		{
			temdot.point[dotnum].cen_w = (pt1.x + pt2.x) / 2.0;
			temdot.point[dotnum].cen_h = (pt1.y + pt2.y) / 2.0;
			temdot.point[dotnum].width = r->width;
			temdot.point[dotnum].height = r->height;
			temdot.point[dotnum].state = 1;
			dotnum++;
		}
	}

	cvReleaseMemStorage(&storage);
	return temdot;
};

void combo_Video_Tracking(
	const char *pIn, const char *pOut, const char* pPolygon,
	bool isdiff, feature *classifier[], int numweak[],
	double smin, double smax, double scale,
	double scalestep, int slidestep, int neighbor, IplImage* bkg1)
{
	// get video information
	CvCapture *capture = cvCaptureFromAVI(pIn);
	double fps = cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
	int frameW = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
	int frameH = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
	int framecount = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);
	CvVideoWriter *writer = cvCreateVideoWriter(pOut, CV_FOURCC('X', 'V', 'I', 'D'), fps / 2, cvSize(frameW, frameH));

	// initialize model
	model md;
	cvInitFont(&md.font, CV_FONT_HERSHEY_PLAIN, 3.0F, 3.0F, 0, 3, 8);

	md.objectInfo[0] = 0;
	md.objectInfo[1] = 1;
	md.objectInfo[2] = 1;

	for (int i = 0; i < TIMEFRAME; i++)
		for (int j = 0; j < CAPACITY; j++)
			md.dot[i].point[j].state = -1;

	ReadClassifier(md.classifier, md.numweak);
	ReadPolygon(pPolygon, md);

	// integral image
	md.source = new integral *[md.roi.height];
	for (int i = 0; i < md.roi.height; i++)
		md.source[i] = new integral[md.roi.width];

	// temporary integral image
	md.s = new integral *[md.roi.height];
	for (int i = 0; i < md.roi.height; i++)
		md.s[i] = new integral[md.roi.width];

	int count = 0;
	IplImage *frame = NULL;
	CvRect roi_draw = md.roi;

	IplImage* temp = cvCreateImage(cvSize(floor(frameW / scale), floor(frameH / scale)), IPL_DEPTH_8U, 3);
	IplImage* bkg2 = cvCreateImage(cvSize(floor(frameW / scale), floor(frameH / scale)), IPL_DEPTH_8U, 1);
	cvResize(bkg1, bkg2);
	IplImage* frame2 = cvCreateImage(cvSize(floor(frameW / scale), floor(frameH / scale)), IPL_DEPTH_8U, 3);
	IplImage* frame_pre = cvCreateImage(cvSize(floor(frameW / scale), floor(frameH / scale)), IPL_DEPTH_8U, 3);
	cvZero(frame_pre);
	while (frame = cvQueryFrame(capture))
	{
		cout << "frame number = " << count << endl;
		if (count % 2 || count < 2) { ++count; continue; }

		//for debugging
		//if(count >= 1000) {break;}

		//cvSetImageROI(frame,roi_draw);
		cvResize(frame, temp);
		if (isdiff)
		{
			DetectionTracking(frame, frame_pre, roi_draw, classifier, numweak, smin, smax, scalestep, slidestep, neighbor, bkg2, md);
		}
		else
		{
			DetectionTracking(frame, NULL, roi_draw, classifier, numweak, smin, smax, scalestep, slidestep, neighbor, bkg2, md);
		}
		//cvRectangle(frame2,cvPoint(roi_draw.x,roi_draw.y),cvPoint(roi_draw.x+roi_draw.width,roi_draw.y+roi_draw.height),CV_RGB(255,0,0),1,8,0);
		//cvResetImageROI(frame);
		cvLine(frame, md.crosslinept1, md.crosslinept2, CV_RGB(255, 0, 0), 3, 8, 0);
		cvWriteFrame(writer, frame);
		cvCopy(temp, frame_pre);

		cout << "===================" << endl;
		printf("%d/%d\t%.1f%%\n", ++count, framecount, 100.0*count / framecount);

	}
	cvReleaseImage(&bkg2);
	cvReleaseImage(&temp);
	cvReleaseImage(&frame_pre);

	// release integral images
	for (int i = 0; i < md.roi.height; i++)
	{
		delete[] md.source[i];
		delete[] md.s[i];
	}
	delete[] md.source;
	delete[] md.s;

	// release polygon
	cvReleaseMat(&md.polygon);

	// release classifier
	for (int i = 0; i < TOTAL_STAGE; i++)
		delete[] md.classifier[i];

	// release capture and writer
	cvReleaseCapture(&capture);
	cvReleaseVideoWriter(&writer);

	return;
}
