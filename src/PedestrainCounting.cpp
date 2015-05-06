#include "PedestrainCounting.h"

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
		h = (int)floor(h / 2);
		countH++;
	}
	while (w > W_thres)
	{
		w = (int)floor(w / 2);
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
		sprintf_s(meanshift_path, TEXT("E:\\Human_detection\\result\\temp_meanshift\\"));
	}
	else
	{
		sprintf_s(meanshift_path, pTemp);
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

void create_meanshift_queue(const TCHAR *pIn, const TCHAR* pOut, int r, int pic_num_all)
{
	CvCapture *capture = cvCaptureFromAVI(pIn);
	double fps = cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
	int frameW = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
	int frameH = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
	int framecount = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);

	int frameall_temp = framecount > 3600 ? 3600 : framecount;

	int pic_num = 1; int count = 0;
	int pic_interval = (int)floor(frameall_temp / pic_num_all);
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
			sprintf_s(infilename, "%s\\%d.jpg", pOut, count);
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
		sprintf_s(infilename, "%s\\%d.jpg", pIn, i + 1);
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