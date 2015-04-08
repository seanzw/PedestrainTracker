#include "HogDetection.h"
using namespace std;
//using namespace cv;

const TCHAR* HOG_CLASSIFIER_PATH = TEXT("..\\HOGClassifier\\");

void cvMouseCallback(int mouseEvent, int x, int y, int flags, void* param)
{
	drawbox* box = (drawbox*)param;
	switch (mouseEvent)
	{
	case CV_EVENT_LBUTTONDOWN:
		box->point1 = cvPoint(x, y);
		box->point2 = cvPoint(x, y);
		box->isDraw = true;
		break;
	case CV_EVENT_MOUSEMOVE:
		box->point2 = cvPoint(x, y);
		break;
	case CV_EVENT_LBUTTONUP:
		box->point2 = cvPoint(x, y);
		cvRectangle(box->image, box->point1, box->point2, CV_RGB(255, 0, 0));
		box->isDraw = false;
		break;
	}
	return;
};

double Get_min_dist(CvPoint center, vector<CvPoint> roi_cen_pre)
{
	int i;
	double dist = 1e7;
	double dist_temp = 0.0;
	for (i = 0; i < roi_cen_pre.size(); i++)
	{
		dist_temp = sqrt((center.x - roi_cen_pre[i].x)*(center.x - roi_cen_pre[i].x) + (center.y - roi_cen_pre[i].y)*(center.y - roi_cen_pre[i].y));
		if (dist_temp < dist)
		{
			dist = dist_temp;
		}
	}
	return dist;
}

int Get_video_framecount(const TCHAR* path2Video)
{
	int nFrames;
	char tempSize[4];
	// Trying to open the video file
	ifstream videoFile(path2Video, ios::in | ios::binary);
	// Checking the availablity of the file
	if (!videoFile) {
		cout << "Couldn't open the input file " << path2Video << endl;
		exit(1);
	}
	// get the number of frames
	videoFile.seekg(0x30, ios::beg);
	videoFile.read(tempSize, 4);
	nFrames = (unsigned char)tempSize[0] + 0x100 * (unsigned char)tempSize[1] + 0x10000 * (unsigned char)tempSize[2] + 0x1000000 * (unsigned char)tempSize[3];
	videoFile.close();
	return nFrames;
}

CvRect Createroi(int x, int y, int roi_h, int roi_w, int edge_width, int height, int width)
{
	int roix, roiy, roih, roiw;
	roix = ((x - edge_width) > 0) ? (x - edge_width) : 1;
	roiy = ((y - edge_width) > 0) ? (y - edge_width) : 1;
	roih = ((roiy + roi_h + 2 * edge_width) > height) ? (height - roiy) : (roi_h + 2 * edge_width);
	roiw = ((roix + roi_w + 2 * edge_width) > width) ? (width - roix) : (roi_w + 2 * edge_width);
	CvRect roi = cvRect(roix, roiy, roiw, roih);
	return roi;
}

void ReadClassifier(feature *classifier[], int numweak[])
{
	printf("Read in classifiers' information...\n");
	for (int i = 0; i < TOTAL_STAGE; i++)
	{
		TCHAR filename[256];
		_stprintf_s(filename, TEXT("%s%d.txt"), HOG_CLASSIFIER_PATH, i + 1);
		//cout<<filename<<endl;
		ifstream wk(filename);
		wk >> numweak[i];
		classifier[i] = new feature[numweak[i]];

		int j = 0;
		while (j < numweak[i])
		{
			wk >> classifier[i][j].h;
			wk >> classifier[i][j].w;
			wk >> classifier[i][j].height;
			wk >> classifier[i][j].width;
			wk >> classifier[i][j].z;
			wk >> classifier[i][j].min;
			wk >> classifier[i][j].max;
			for (int k = 0; k < 36; k++)
			{
				wk >> classifier[i][j].projection[k];
			}
			for (int k = 0; k < 100; k++)
			{
				wk >> classifier[i][j].histogram[k];
			}
			j++;
			if (j == numweak[i])
			{
				wk >> classifier[i][j - 1].threshold;
			}
		}
		wk.close();
	}
	printf("Reading process completed!\n");
}

bool judge_pedestrian(
	IplImage* frame, IplImage* frame_pre,
	CvRect roi, double thres_max, double thres_min,
	double thres_binary, double thres_ratio)
{
	int frameH = frame->height;
	int frameW = frame->width;
	int h = roi.height; int w = roi.width;
	bool jud1 = (h > w);
	bool jud2 = (h*w >= thres_min * frameH * frameW);
	bool jud3 = (h*w <= thres_max * frameH * frameW);

	bool res;

	if (frame_pre == NULL)
	{
		res = jud1 && jud2 && jud3;
	}
	else
	{
		//process current frame
		IplImage* img_dst = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 3);
		cvSmooth(frame, img_dst, CV_GAUSSIAN, GAUSSIAN_SMOOTH_R, GAUSSIAN_SMOOTH_R, 0, 0);
		// get difference between current pictures and background
		IplImage* img_test = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
		cvCvtColor(img_dst, img_test, CV_RGB2GRAY);
		CvMat* img_testMat = cvCreateMat(frame->height, frame->width, CV_32FC1);
		cvConvert(img_test, img_testMat);

		//process pre frame
		IplImage* img_dst_pre = cvCreateImage(cvGetSize(frame_pre), IPL_DEPTH_8U, 3);
		cvSmooth(frame_pre, img_dst_pre, CV_GAUSSIAN, GAUSSIAN_SMOOTH_R, GAUSSIAN_SMOOTH_R, 0, 0);
		// get difference between current pictures and background
		IplImage* img_test_pre = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
		cvCvtColor(img_dst_pre, img_test_pre, CV_RGB2GRAY);
		CvMat* img_testMat_pre = cvCreateMat(frame->height, frame->width, CV_32FC1);
		cvConvert(img_test_pre, img_testMat_pre);

		CvMat* img_diff1Mat = cvCreateMat(frame->height, frame->width, CV_32FC1);
		cvAbsDiff(img_testMat, img_testMat_pre, img_diff1Mat);

		// get bwlabels from binary picture
		IplImage* img_binary1 = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
		cvThreshold(img_diff1Mat, img_binary1, thres_binary, 255.0, CV_THRESH_BINARY);

		cvSetImageROI(img_binary1, roi);
		CvScalar sum_binary = cvSum(img_binary1);
		double ratio_sum = sum_binary.val[0] / h / w;
		bool jud4 = (ratio_sum >= thres_ratio);

		//bool jud4 = true;

		res = jud1 && jud2 && jud3 && jud4;
		cvReleaseImage(&img_dst_pre);
		cvReleaseImage(&img_dst);
		cvReleaseImage(&img_test);
		cvReleaseImage(&img_test_pre);
		cvReleaseImage(&img_binary1);
		cvReleaseMat(&img_testMat);
		cvReleaseMat(&img_testMat_pre);
		cvReleaseMat(&img_diff1Mat);
	}
	return res;
};

CvRect ReadPolygon(const TCHAR *pPolygon)
{
	TCHAR buffer[32];
	FILE *pFile;
	fopen_s(&pFile, pPolygon, "r");

	fgets(buffer, 31, pFile); // ImageWidth
	fgets(buffer, 31, pFile); // ImageHeight
	fgets(buffer, 31, pFile); // PolygonPointCount
	int count = atoi(strrchr(buffer, '=') + 1);

	// read polygon points
	int x, y;
	CvPoint ding1, ding2;
	fgets(buffer, 31, pFile);
	sscanf_s(buffer, "(%d,%d)", &x, &y);
	ding1.x = ding2.x = x;
	ding1.y = ding2.y = y;

	for (int k = 1; k < count; ++k)
	{
		fgets(buffer, 31, pFile);
		sscanf_s(buffer, "(%d,%d)", &x, &y);
		ding1.x = (ding1.x < x) ? ding1.x : x;
		ding1.y = (ding1.y < y) ? ding1.y : y;
		ding2.x = (ding2.x > x) ? ding2.x : x;
		ding2.y = (ding2.y > y) ? ding2.y : y;
	}
	++ding2.x;
	++ding2.y;
	fclose(pFile);

	return cvRect(ding1.x, ding1.y, ding2.x - ding1.x, ding2.y - ding1.y);
}

void DetectPicture(
	const TCHAR *pIn, const TCHAR *pOut, const TCHAR *pPolygon,
	feature *classifier[], int numweak[],
	double smin, double smax,
	double scalestep, int slidestep, int neighbor)
{
	// read image
	IplImage *frame = cvLoadImage(pIn);

	// read designated polygon
	CvRect roi = pPolygon ? ReadPolygon(pPolygon) : cvRect(0, 0, frame->width, frame->height);

	// prepare data buffer
	IplImage *img = cvCreateImage(cvSize(roi.width, roi.height), IPL_DEPTH_8U, 3);
	IplImage *gray = cvCreateImage(cvSize(roi.width, roi.height), IPL_DEPTH_8U, 1);

	// integral image
	integral **source = new integral *[roi.height];
	for (int i = 0; i < roi.height; i++)
		source[i] = new integral[roi.width];

	// temporary integral image
	integral **s = new integral *[roi.height];
	for (int i = 0; i < roi.height; i++)
		s[i] = new integral[roi.width];

	cvSetImageROI(frame, roi);
	cvCopy(frame, img);
	cvCvtColor(img, gray, CV_RGB2GRAY);
	Detection(img, gray, classifier, numweak, source, s, smin, smax, scalestep, slidestep, neighbor);
	cvCopy(img, frame);
	cvResetImageROI(frame);
	cvSaveImage(pOut, frame);

	// release integral images
	for (int i = 0; i < roi.height; i++)
	{
		delete[] source[i];
		delete[] s[i];
	}
	delete[] source;
	delete[] s;

	// release data buffer
	cvReleaseImage(&frame);
	cvReleaseImage(&img);
	cvReleaseImage(&gray);
}

IplImage* combo_DetectPicture(
	IplImage* frame,
	feature *classifier[], int numweak[],
	double smin, double smax,
	double scalestep, int slidestep, int neighbor, IplImage* bkg1)
{
	//processing picture
	IplImage* img_dst = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 3);
	cvSmooth(frame, img_dst, CV_GAUSSIAN, GAUSSIAN_SMOOTH_R, GAUSSIAN_SMOOTH_R, 0, 0);

	// get difference between current pictures and background
	IplImage* img_test = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
	cvCvtColor(img_dst, img_test, CV_RGB2GRAY);
	CvMat* img_testMat = cvCreateMat(frame->height, frame->width, CV_32FC1);
	cvConvert(img_test, img_testMat);
	CvMat* img_diff1Mat = cvCreateMat(frame->height, frame->width, CV_32FC1);
	CvMat* img_bkg1Mat = cvCreateMat(bkg1->height, bkg1->width, CV_32FC1);
	cvConvert(bkg1, img_bkg1Mat);
	IplImage* img_binary1 = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
	cvAbsDiff(img_testMat, img_bkg1Mat, img_diff1Mat);

	// get bwlabels from binary picture
	cvThreshold(img_diff1Mat, img_binary1, THRES_DIFF_BINARY, 255.0, CV_THRESH_BINARY);

	//find candidate Polygons
	int num_Polygon = 0;
	CvSeq* c;
	cv::Mat img_diff = cv::cvarrToMat(img_binary1);
	img_diff = img_diff > 0;
	c = ConnectedComponents(img_diff, 0, THRES_PERIMETER_RATIO, 1, cv::Rect(), cv::Point(-1, -1));

	// if exist cadidates of roi
	if (c != NULL)
	{
		for (; c != NULL; ++num_Polygon, c = c->h_next)
		{
			// create roi in each frame
			CvRect roi = cvBoundingRect(c);

			if (roi.height*roi.width <= THRES_AREA_ROI_MAX*frame->height*frame->width
				&& roi.height*roi.width >= THRES_AREA_ROI_MIN*frame->height*frame->width && roi.height > roi.width)
			{
				roi = Createroi(roi.x, roi.y, roi.height, roi.width, 10, frame->height, frame->width);
				cvRectangle(frame, cvPoint(roi.x, roi.y), cvPoint(roi.x + roi.width, roi.y + roi.height), CV_RGB(0, 255, 0), 2, 8, 0);
				//cout<<"Rect "<<num_Polygon+1<<": "<<roi.x<<","<<roi.y<<","<<roi.height<<","<<roi.width<<endl;
			}
			else
			{
				roi = Createroi(roi.x, roi.y, roi.height, roi.width, EDGE_WIDTH, frame->height, frame->width);
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

				Detection(img, gray, classifier, numweak, source, s, smin, smax, scalestep, slidestep, neighbor);

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

	return frame;
};

IplImage* combo_DetectPicture(
	IplImage* frame, IplImage* frame_pre, CvRect roi0,
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

	// if exist cadidates of roi
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
				roi = Createroi(roi.x, roi.y, roi.height, roi.width, 10, frame->height, frame->width);
				cvRectangle(frame, cvPoint(roi.x, roi.y), cvPoint(roi.x + roi.width, roi.y + roi.height), CV_RGB(0, 255, 0), 2, 8, 0);
				cout << "Rect " << num_Polygon + 1 << ": Add directly!" << endl;
				//cout<<"Rect "<<num_Polygon+1<<": "<<roi.x<<","<<roi.y<<","<<roi.height<<","<<roi.width<<endl;
			}
			else
			{
				cvSetImageROI(frame, roi0);
				roi = Createroi(roi.x, roi.y, roi.height, roi.width, EDGE_WIDTH, frame->height, frame->width);
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

				Detection(img, gray, classifier, numweak, source, s, smin, smax, scalestep, slidestep, neighbor);

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
	return frame;
}

void combo_DetectPicture(
	const TCHAR *pIn, const TCHAR *pOut,
	feature *classifier[], int numweak[],
	double smin, double smax,
	double scalestep, int slidestep, int neighbor, IplImage* bkg1)
{
	// read image
	IplImage *frame = cvLoadImage(pIn);

	//processing picture
	IplImage* img_dst = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 3);
	cvSmooth(frame, img_dst, CV_GAUSSIAN, GAUSSIAN_SMOOTH_R, GAUSSIAN_SMOOTH_R, 0, 0);

	// get difference between current pictures and background
	IplImage* img_test = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
	cvCvtColor(img_dst, img_test, CV_RGB2GRAY);
	CvMat* img_testMat = cvCreateMat(frame->height, frame->width, CV_32FC1);
	cvConvert(img_test, img_testMat);
	CvMat* img_diff1Mat = cvCreateMat(frame->height, frame->width, CV_32FC1);
	//IplImage* img_bkg1 = cvCreateImage(cvGetSize(bkg1),IPL_DEPTH_8U,1);
	//cvCvtColor(bkg1,img_bkg1,CV_RGB2GRAY);
	CvMat* img_bkg1Mat = cvCreateMat(bkg1->height, bkg1->width, CV_32FC1);
	cvConvert(bkg1, img_bkg1Mat);
	IplImage* img_binary1 = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
	cvAbsDiff(img_testMat, img_bkg1Mat, img_diff1Mat);

	// get bwlabels from binary picture
	cvThreshold(img_diff1Mat, img_binary1, THRES_DIFF_BINARY, 255.0, CV_THRESH_BINARY);

	//find candidate Polygons
	int num_Polygon = 0;
	CvSeq* c;
	cv::Mat img_diff = cv::cvarrToMat(img_binary1);
	img_diff = img_diff > 0;
	c = ConnectedComponents(img_diff, 0, THRES_PERIMETER_RATIO, 1, cv::Rect(), cv::Point(-1, -1));

	// if exist cadidates of roi
	if (c != NULL)
	{
		for (; c != NULL; ++num_Polygon, c = c->h_next)
		{
			// create roi in each frame
			CvRect roi = cvBoundingRect(c);

			if (roi.height*roi.width <= THRES_AREA_ROI_MAX*frame->height*frame->width
				&& roi.height*roi.width >= THRES_AREA_ROI_MIN*frame->height*frame->width && roi.height > roi.width)
			{
				roi = Createroi(roi.x, roi.y, roi.height, roi.width, 10, frame->height, frame->width);
				cvRectangle(frame, cvPoint(roi.x, roi.y), cvPoint(roi.x + roi.width, roi.y + roi.height), CV_RGB(0, 255, 0), 2, 8, 0);
			}
			else
			{
				roi = Createroi(roi.x, roi.y, roi.height, roi.width, EDGE_WIDTH, frame->height, frame->width);
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

				Detection(img, gray, classifier, numweak, source, s, smin, smax, scalestep, slidestep, neighbor);

				cout << "Finish Detection Rect " << num_Polygon + 1 << "!" << endl;

				cvCopy(img, frame);
				cvResetImageROI(frame);
				cvSaveImage(pOut, frame);

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
	cvReleaseImage(&frame);
	cvReleaseImage(&img_binary1);
	cvReleaseImage(&img_dst);
	cvReleaseImage(&img_test);
	cvReleaseMat(&img_bkg1Mat);
	cvReleaseMat(&img_diff1Mat);
	cvReleaseMat(&img_testMat);
};

IplImage* combo_DetectPicture_diff(
	IplImage* frame, IplImage* frame_pre,
	feature *classifier[], int numweak[],
	double smin, double smax, double thres_binary_frame,
	double scalestep, int slidestep, int neighbor)
{
	//find candidate Polygons
	int num_Polygon = 0;
	CvSeq* c;

	c = get_framediff_components(frame, frame_pre, thres_binary_frame);

	cvWaitKey(0);
	cvDestroyWindow("img_smooth");
	cvDestroyWindow("img_diff");
	cvDestroyWindow("binary");


	// if exist cadidates of roi
	if (c != NULL)
	{
		for (; c != NULL; ++num_Polygon, c = c->h_next)
		{
			// create roi in each frame
			CvRect roi = cvBoundingRect(c);

			if (roi.height*roi.width <= THRES_AREA_ROI_MAX*frame->height*frame->width
				&& roi.height*roi.width >= THRES_AREA_ROI_MIN*frame->height*frame->width && roi.height > roi.width)
			{
				roi = Createroi(roi.x, roi.y, roi.height, roi.width, 10, frame->height, frame->width);
				cvRectangle(frame, cvPoint(roi.x, roi.y), cvPoint(roi.x + roi.width, roi.y + roi.height), CV_RGB(0, 255, 0), 2, 8, 0);
			}
			else
			{
				roi = Createroi(roi.x, roi.y, roi.height, roi.width, EDGE_WIDTH, frame->height, frame->width);
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

				Detection(img, gray, classifier, numweak, source, s, smin, smax, scalestep, slidestep, neighbor);

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
	//cvReleaseImage(&frame);

	return frame;
};

void DetectVideo(
	const TCHAR *pIn, const TCHAR *pOut, const TCHAR *pPolygon,
	feature *classifier[], int numweak[],
	double smin, double smax, double scale,
	double scalestep, int slidestep, int neighbor)
{
	// get video information
	CvCapture *capture = cvCaptureFromAVI(pIn);
	double fps = cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
	int frameW = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
	int frameH = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
	int framecount = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);
	CvVideoWriter *writer = cvCreateVideoWriter(pOut, CV_FOURCC('X', 'V', 'I', 'D'), fps / 2, cvSize(floor(frameW / scale), floor(frameH / scale)));

	// read designated polygon
	CvRect roi = pPolygon ? ReadPolygon(pPolygon) : cvRect(0, 0, floor(frameW / scale), floor(frameH / scale));

	// prepare data buffer
	IplImage *img = cvCreateImage(cvSize(roi.width, roi.height), IPL_DEPTH_8U, 3);
	IplImage *gray = cvCreateImage(cvSize(roi.width, roi.height), IPL_DEPTH_8U, 1);

	// integral image
	integral **source = new integral *[roi.height];
	for (int i = 0; i < roi.height; i++)
		source[i] = new integral[roi.width];

	// temporary integral image
	integral **s = new integral *[roi.height];
	for (int i = 0; i < roi.height; i++)
		s[i] = new integral[roi.width];

	int count = 0;
	IplImage *frame = NULL;
	while (frame = cvQueryFrame(capture))
	{

		cout << "frame number = " << count << endl;

		if (count % 2) { ++count; continue; }

		//for debugging
		//if(count >= 1000) {break;}

		//cvSetImageROI(frame,roi);
		cvResize(frame, img);
		cvCvtColor(img, gray, CV_RGB2GRAY);
		cout << "Processing frame " << count << endl;
		Detection(img, gray, classifier, numweak, source, s, smin, smax, scalestep, slidestep, neighbor);

		//cvNamedWindow("test");
		//cvShowImage("test",img);
		//cvWaitKey(0);
		//cvDestroyWindow("test");

		cvCopy(img, frame);
		cvWriteFrame(writer, frame);
		cout << "===================" << endl;
		printf("%d/%d\t%.1f%%\n", ++count, framecount, 100.0*count / framecount);
	}

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

	// release capture and writer
	cvReleaseCapture(&capture);
	cvReleaseVideoWriter(&writer);
};

double combo_DetectVideo(
	const TCHAR *pIn, const TCHAR *pOut, bool isdraw,
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
	CvVideoWriter *writer = cvCreateVideoWriter(pOut, CV_FOURCC('X', 'V', 'I', 'D'), fps / 2, cvSize(floor(frameW / scale), floor(frameH / scale)));

	double exe_time = 0.0;
	int count = 0;
	IplImage *frame = NULL;
	TCHAR infilename[1000];

	cout << bkg1->height << endl << bkg1->width << endl;
	cout << "good" << endl;

	drawbox box;
	CvRect roi_draw = cvRect(0, 0, floor(frameW / scale), floor(frameH / scale));
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

		if (isdraw && count == 2)
		{
			LARGE_INTEGER freq;
			LARGE_INTEGER start_t, stop_t;

			QueryPerformanceFrequency(&freq);
			QueryPerformanceCounter(&start_t);
			box.image = frame;
			box.temp = cvCloneImage(box.image);
			box.isDraw = false;

			// register a mouse callback function  
			cvNamedWindow("exam", CV_WINDOW_AUTOSIZE);
			cvSetMouseCallback("exam", cvMouseCallback, &box);
			while (1)
			{
				cvCopy(box.image, box.temp);
				if (box.isDraw)
				{
					cvRectangle(box.temp, box.point1, box.point2, CV_RGB(255, 255, 255));
				}
				cvShowImage("exam", box.temp);
				if (cvWaitKey(1) == 13)  break;
			}
			cvDestroyWindow("exam");

			roi_draw = cvRect(min(floor(box.point1.x / scale), floor(box.point2.x / scale)), min(floor(box.point1.y / scale), floor(box.point2.y / scale)),
				floor(abs(box.point1.x - box.point2.x + 1) / scale), floor(abs(box.point1.y - box.point2.y + 1) / scale));
			QueryPerformanceCounter(&stop_t);
			exe_time = double(stop_t.QuadPart - start_t.QuadPart) / freq.QuadPart;
			printf("Drawing box's time = %fs.\n", exe_time);
		}

		// for debugging
		//if(count >= 1000) {break;}

		//cvSetImageROI(frame,roi_draw);
		cvResize(frame, temp);
		if (isdiff)
		{
			frame2 = combo_DetectPicture(temp, frame_pre, roi_draw, classifier, numweak, smin, smax, scalestep, slidestep, neighbor, bkg2);
		}
		else
		{
			frame2 = combo_DetectPicture(temp, NULL, roi_draw, classifier, numweak, smin, smax, scalestep, slidestep, neighbor, bkg2);
		}
		cvRectangle(frame2, cvPoint(roi_draw.x, roi_draw.y), cvPoint(roi_draw.x + roi_draw.width, roi_draw.y + roi_draw.height), CV_RGB(255, 0, 0), 1, 8, 0);
		cvWriteFrame(writer, frame2);
		cvCopy(temp, frame_pre);

		cout << "===================" << endl;
		printf("%d/%d\t%.1f%%\n", ++count, framecount, 100.0*count / framecount);

	}
	cvReleaseImage(&bkg2);
	cvReleaseImage(&temp);
	cvReleaseImage(&frame_pre);
	if (isdraw)
	{
		cvReleaseImage(&box.temp);
	}
	cvReleaseCapture(&capture);
	cvReleaseVideoWriter(&writer);
	return exe_time;
};

void combo_DetectVideo_framediff(
	const TCHAR *pIn, const TCHAR *pOut,
	feature *classifier[], int numweak[],
	double smin, double smax, double thres_binary_frame,
	double scalestep, int slidestep, int neighbor, IplImage* bkg1)
{
	// get video information
	CvCapture *capture = cvCaptureFromAVI(pIn);
	double fps = cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
	int frameW = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
	int frameH = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
	int framecount = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);
	CvVideoWriter *writer = cvCreateVideoWriter(pOut, CV_FOURCC('X', 'V', 'I', 'D'), fps / 2, cvSize(frameW, frameH));

	int count = 0;
	IplImage *frame = NULL;
	TCHAR infilename[1000];

	IplImage* temp = cvCreateImage(cvSize(frameW, frameH), IPL_DEPTH_8U, 3);
	IplImage* frame_pre = cvCreateImage(cvSize(frameW, frameH), IPL_DEPTH_8U, 3);
	IplImage* frame2 = cvCreateImage(cvSize(frameW, frameH), IPL_DEPTH_8U, 3);
	while (frame = cvQueryFrame(capture))
	{
		if (count == 0)
		{
			cvCopy(frame, temp);
			cvCopy(frame, frame_pre);
			cout << "frame number = " << count << endl;
			if (count % 2) { ++count; continue; }

			//for debugging
			//if(count >= 1000) {break;}


			frame2 = combo_DetectPicture(temp, classifier, numweak, smin, smax, scalestep, slidestep, neighbor, bkg1);

			cvWriteFrame(writer, frame2);

			cout << "===================" << endl;
			printf("%d/%d\t%.1f%%\n", ++count, framecount, 100.0*count / framecount);

			//cvCopy(temp,frame_pre);
		}
		else
		{
			cvCopy(frame, temp);
			cout << "frame number = " << count << endl;
			if (count % 2) { ++count; continue; }

			//if(count >= 1000) {break;}


			frame2 = combo_DetectPicture_diff(temp, frame_pre, classifier, numweak, smin, smax, thres_binary_frame, scalestep, slidestep, neighbor);

			cvWriteFrame(writer, frame2);

			cout << "===================" << endl;
			printf("%d/%d\t%.1f%%\n", ++count, framecount, 100.0*count / framecount);

			cvCopy(temp, frame_pre);
		}

	}

	cvReleaseImage(&temp);
	cvReleaseImage(&frame_pre);
	cvReleaseCapture(&capture);
	cvReleaseVideoWriter(&writer);
};

void readin_tuples(TCHAR pIn[], Tuple* &tuples, int pic_num_all, CvRect roi, int frameH, int frameW)
{
	int h = roi.height;
	int w = roi.width;
	//Tuple tuple1;

	TCHAR infilename[100];

	IplImage* img_meanshift = cvCreateImage(cvSize(frameW, frameH), IPL_DEPTH_8U, 1);
	IplImage* img_meanshift_roi = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 1);
	CvMat* Matdata = cvCreateMat(h, w, CV_32FC1);

	for (int i = 0; i < pic_num_all; i++)
	{
		_stprintf_s(infilename, "%s\\%d.jpg", pIn, i + 1);
		cout << infilename << endl;

		img_meanshift = cvLoadImage(infilename, 0);
		cvSetImageROI(img_meanshift, roi);
		cvCopy(img_meanshift, img_meanshift_roi);
		cvResetImageROI(img_meanshift);

		cvConvert(img_meanshift_roi, Matdata);
		readin_data(tuples[i], h*w, Matdata);

		// cvLoadImage 有内存泄露！！！
		cvReleaseImage(&img_meanshift);

	}

	cvReleaseImage(&img_meanshift);
	cvReleaseImage(&img_meanshift_roi);
	cvReleaseMat(&Matdata);

};

void create_meanshift_queue(const TCHAR *pIn, const TCHAR* pOut, int r, int pic_num_all)
{
	CvCapture *capture = cvCaptureFromAVI(pIn);
	double fps = cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
	int frameW = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
	int frameH = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
	int framecount = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);

	int frameall_temp = framecount > 3600 ? 3600 : framecount;

	int pic_num = 1; int count = 0;
	int pic_interval = floor(frameall_temp / pic_num_all);
	pic_interval = pic_interval > 35 ? 35 : pic_interval;

	IplImage* img_meanshift = cvCreateImage(cvSize(frameW, frameH), IPL_DEPTH_8U, 1);
	IplImage* frame_gray = cvCreateImage(cvSize(frameW, frameH), IPL_DEPTH_8U, 1);
	IplImage* img_temp = cvCreateImage(cvSize(frameW, frameH), IPL_DEPTH_8U, 1);
	IplImage* img_sum = cvCreateImage(cvSize(frameW, frameH), IPL_DEPTH_32F, 1);
	IplImage* img_zero = cvCreateImage(cvSize(frameW, frameH), IPL_DEPTH_8U, 1);
	IplImage* temp = cvCreateImage(cvSize(frameW, frameH), IPL_DEPTH_8U, 1);
	cvZero(img_zero);
	cvZero(img_sum);


	IplImage* frame = NULL;
	TCHAR infilename[256];
	while (frame = cvQueryFrame(capture))
	{
		if (!(pic_num%pic_interval))
		{
			++count;

			cvCvtColor(frame, frame_gray, CV_RGB2GRAY);
			cvConvert(frame_gray, img_temp);
			cvAcc(img_temp, img_sum);
			cvConvertScale(img_sum, temp, 1.0 / pic_interval);

			cout << "No." << count << ": Current processing frame " << pic_num << endl;

			img_meanshift = pic_meanshift(temp, r);
			_stprintf_s(infilename, "%s\\%d.jpg", pOut, count);
			cvSaveImage(infilename, img_meanshift);

			cvZero(img_sum);

			cvReleaseImage(&img_meanshift);
			//cvReleaseImage(&temp);
			++pic_num;
		}
		else
		{
			cvCvtColor(frame, frame_gray, CV_RGB2GRAY);
			cvConvert(frame_gray, img_temp);
			cvAcc(img_temp, img_sum);
			++pic_num;
			continue;
		}
		if (pic_num > pic_interval*pic_num_all)
		{
			break;
		}

	}
	cvReleaseImage(&img_meanshift);
	cvReleaseImage(&img_sum);
	cvReleaseImage(&img_temp);
	cvReleaseImage(&temp);
	cvReleaseImage(&img_zero);
	//cvReleaseImage(&temp);
	cvReleaseCapture(&capture);

	//delete [] infilename;

	return;
};

IplImage* video_bkg_detect(
	const TCHAR *pIn, const TCHAR* pTemp, int num_cluster, int r, int pic_num_all,
	int H_thres, int W_thres)
{
	CvCapture *capture = cvCaptureFromAVI(pIn);
	int frameW = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
	int frameH = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
	cvReleaseCapture(&capture);

	//frameH = 480; frameW = 640;
	IplImage* background = cvCreateImage(cvSize(frameW, frameH), IPL_DEPTH_8U, 1);

	int h = frameH; int w = frameW;
	int countH = 0; int countW = 0;
	while (h > H_thres)
	{
		h = floor(h / 2);
		countH++;
	}
	while (w > W_thres)
	{
		w = floor(w / 2);
		countW++;
	}

	IplImage* bkg_temp = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 1);
	int bkg_step = bkg_temp->widthStep;
	uchar* bkg_data = (uchar*)bkg_temp->imageData;

	cout << "Processing patch's size = (" << h << "," << w << ")" << endl;

	//获得meanshift处理后的图片
	TCHAR meanshift_path[256];
	if (pTemp == NULL)
	{
		_stprintf_s(meanshift_path, TEXT("E:\\Human_detection\\result\\temp_meanshift\\"));
	}
	else
	{
		_stprintf_s(meanshift_path, pTemp);
	}

	int flag = CreateDirectory(meanshift_path, NULL);
	create_meanshift_queue(pIn, meanshift_path, r, pic_num_all);

	Tuple* tuples = new Tuple[pic_num_all];
	Tuple* means = new Tuple[num_cluster];

	for (int i = 0; i < pic_num_all; i++)
	{
		init_Tuple(tuples[i], h*w);
	}
	for (int i = 0; i < num_cluster; i++)
	{
		init_Tuple(means[i], h*w);
	}
	for (int hh = 0; hh < pow(2, countH); hh++)
	{
		for (int ww = 0; ww < pow(2, countW); ww++)
		{
			//读取元素
			cout << "Processing patch (" << hh << "," << ww << ")" << endl;

			//Tuple* tuples = new Tuple[pic_num_all];
			for (int i = 0; i < pic_num_all; i++)
			{
				delete_tuple(tuples[i]);
				init_Tuple(tuples[i], h*w);
			}
			CvRect roi = cvRect(ww*w, hh*h, w, h);
			readin_tuples(meanshift_path, tuples, pic_num_all, roi, frameH, frameW);

			//kmeans聚类
			vector<int> *labels = new vector<int>[num_cluster];
			labels = KMeans(tuples, num_cluster, THRES_Diff, pic_num_all);

			row_data row_current;
			vector<row_data> row_ordered;
			//Tuple* means = new Tuple[num_cluster];
			for (int i = 0; i < num_cluster; i++)
			{
				delete_tuple(means[i]);
				init_Tuple(means[i], h*w);
				cal_mean(tuples, means[i], labels[i]);
				row_current.row = labels[i].size();
				row_current.label = i;
				row_ordered.push_back(row_current);
			}

			sort(row_ordered.begin(), row_ordered.end(), greatermark);

			cout << "The most probable background is cluster " << row_ordered[0].label << endl;

			//获得局部背景并复制
			for (int ii = 0; ii < h; ii++)
			{
				for (int jj = 0; jj < w; jj++)
				{
					bkg_data[ii*bkg_step + jj] = max(0, min(255, (int)means[row_ordered[0].label].data[ii*w + jj]));
				}
			}

			cvSetImageROI(background, roi);
			cvCopy(bkg_temp, background);
			cvResetImageROI(background);

		}
	}

	//清空内存及文件
	TCHAR order[300];
	for (int ii = 0; ii < num_cluster; ii++)
	{
		delete_tuple(means[ii]);
	}
	delete[]means;
	for (int ii = 0; ii < pic_num_all; ii++)
	{
		//sprintf(order,"del %s%d.jpg",meanshift_path,ii);
		//system("order");
		delete_tuple(tuples[ii]);
	}
	delete[] tuples;
	//if(flag)
	//{
	//	RemoveDirectory(meanshift_path);
	//}

	cvReleaseImage(&bkg_temp);
	return background;
};

CvSeq* get_framediff_components(
	IplImage* &frame, IplImage* frame_pre,
	double thres_binary_frame)
{
	//process current frame
	IplImage* img_dst = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 3);
	cvSmooth(frame, img_dst, CV_GAUSSIAN, GAUSSIAN_SMOOTH_R, GAUSSIAN_SMOOTH_R, 0, 0);
	// get difference between current pictures and background
	IplImage* img_test = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
	cvCvtColor(img_dst, img_test, CV_RGB2GRAY);
	CvMat* img_testMat = cvCreateMat(frame->height, frame->width, CV_32FC1);
	cvConvert(img_test, img_testMat);

	//process pre frame
	IplImage* img_dst_pre = cvCreateImage(cvGetSize(frame_pre), IPL_DEPTH_8U, 3);
	cvSmooth(frame_pre, img_dst_pre, CV_GAUSSIAN, GAUSSIAN_SMOOTH_R, GAUSSIAN_SMOOTH_R, 0, 0);
	// get difference between current pictures and background
	IplImage* img_test_pre = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
	cvCvtColor(img_dst_pre, img_test_pre, CV_RGB2GRAY);
	CvMat* img_testMat_pre = cvCreateMat(frame->height, frame->width, CV_32FC1);
	cvConvert(img_test_pre, img_testMat_pre);

	CvMat* img_diff1Mat = cvCreateMat(frame->height, frame->width, CV_32FC1);
	cvAbsDiff(img_testMat, img_testMat_pre, img_diff1Mat);

	// get bwlabels from binary picture
	IplImage* img_binary1 = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
	cvThreshold(img_diff1Mat, img_binary1, thres_binary_frame, 255.0, CV_THRESH_BINARY);

	//cvSaveImage("E:\\Human_detection\\result\\diff_frame_130525.jpg",img_binary1);

	//find candidate Polygons
	int num_Polygon = 0;
	double dist_min = (double)frame->width;
	double dist_temp = 0.0;
	CvSeq* c;
	cv::Mat img_diff = cv::cvarrToMat(img_binary1);
	img_diff = img_diff > 0;
	c = ConnectedComponents(img_diff, 0, THRES_PERIMETER_RATIO, 1, cv::Rect(), cv::Point(-1, -1));

	cvReleaseImage(&img_binary1);
	cvReleaseImage(&img_dst);
	cvReleaseImage(&img_dst_pre);
	cvReleaseImage(&img_test_pre);
	cvReleaseImage(&img_test);
	cvReleaseMat(&img_testMat);
	cvReleaseMat(&img_testMat_pre);
	cvReleaseMat(&img_diff1Mat);

	return c;
};