#include "meanshift.h"

using namespace std;
//using namespace cv;

const double pi = 3.14159265359;
const double diff_w = 0.01;

IplImage* create_input4meanshift(IplImage* img, int r)
{
	int imgh = img->height;
	int imgw = img->width;
	//IplImage* img_gray = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,1);
	//cvCvtColor(img,img_gray,CV_RGB2GRAY);
	IplImage* img_in = cvCreateImage(cvSize(imgw + 2 * r + 1, imgh + 2 * r + 1), IPL_DEPTH_8U, 1);
	IplImage* img_in_mask = cvCreateImage(cvGetSize(img_in), IPL_DEPTH_8U, 1);

	//set input image
	cvSetImageROI(img_in, cvRect(r, r, imgw, imgh));
	cvCopy(img, img_in);
	cvResetImageROI(img_in);

	cvSetImageROI(img_in, cvRect(r, 0, imgw, r));
	cvSetImageROI(img, cvRect(0, 0, imgw, r));
	cvCopy(img, img_in);
	cvResetImageROI(img);
	cvResetImageROI(img_in);
	cvCopy(img_in, img_in_mask);

	cvSetImageROI(img_in, cvRect(imgw + r, 0, r + 1, imgh + r));
	cvSetImageROI(img_in_mask, cvRect(imgw - 1, 0, r + 1, imgh + r));
	cvCopy(img_in_mask, img_in);
	cvResetImageROI(img_in);	cvResetImageROI(img_in_mask);
	cvCopy(img_in, img_in_mask);

	cvSetImageROI(img_in, cvRect(r, imgh + r, imgw + r + 1, r + 1));
	cvSetImageROI(img_in_mask, cvRect(r, imgh - 1, imgw + r + 1, r + 1));
	cvCopy(img_in_mask, img_in);
	cvResetImageROI(img_in);	cvResetImageROI(img_in_mask);
	cvCopy(img_in, img_in_mask);

	cvSetImageROI(img_in, cvRect(0, 0, r, imgh + 2 * r + 1));
	cvSetImageROI(img_in_mask, cvRect(r, 0, r, imgh + 2 * r + 1));
	cvCopy(img_in_mask, img_in);
	cvResetImageROI(img_in);	cvResetImageROI(img_in_mask);
	cvCopy(img_in, img_in_mask);

	//cvReleaseImage(&img);
	//cvReleaseImage(&img_gray);
	cvReleaseImage(&img_in_mask);

	return img_in;
};

double meanshift(double* serArr, int p, int length)
{
	double* K = new double[length];
	double w = 0.0;
	double* temp = new double[length];
	double sumK = 0.0;
	//int count = 0;

	double pre_w = (serArr[p]);
	while (1)
	{
		//count++;
		//cout<<count<<" time iterate"<<endl;
		for (int i = 0; i < length; i++)
		{
			temp[i] = serArr[i];
			temp[i] = temp[i] - pre_w;
			if (i != p)
			{
				temp[i] /= (i - p);
			}
			temp[i] *= temp[i];
			K[i] = (1 / sqrt(2 * pi))*exp(-0.5*temp[i]);
			sumK += K[i];
		}
		for (int i = 0; i < length; i++)
		{
			w += serArr[i] * K[i] / sumK;
		}
		if (abs(w - pre_w) < diff_w)
		{
			break;
		}
		pre_w = w;
		sumK = 0.0;
		w = 0.0;
	}
	//cout<<"Iteration times = "<<count<<endl;
	delete[] K;
	delete[] temp;
	return w;
};

IplImage* pic_meanshift(IplImage* img, int r)
{
	int imgh = img->height;
	int imgw = img->width;
	IplImage* img_meanshift = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
	IplImage* img_in = cvCreateImage(cvSize(imgw + 2 * r + 1, imgh + 2 * r + 1), IPL_DEPTH_8U, 1);
	IplImage* img_out = cvCreateImage(cvGetSize(img_in), IPL_DEPTH_8U, 1);
	IplImage* img_result = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);

	img_in = create_input4meanshift(img, r);
	cvCopy(img_in, img_out);

	for (int i = 1 + r; i <= (imgh + r); i++)
	{
		for (int j = 1 + r; j <= (imgw + r); j++)
		{
			double * serArr = new double[(2 * r + 1)*(2 * r + 1)];
			for (int n = 0; n < (2 * r + 1); n++)
			{
				for (int m = 0; m < (2 * r + 1); m++)
				{
					serArr[n*(2 * r + 1) + m] = cvGet2D(img_in, i - r - 1 + n, j - r - 1 + m).val[0];
				}
			}
			cvSet2D(img_out, i - 1, j - 1, cvScalar(meanshift(serArr, 2 * r*r + 2 * r, (2 * r + 1)*(2 * r + 1))));
			delete[] serArr;
		}
	}

	cvSetImageROI(img_out, cvRect(r, r, imgw, imgh));
	cvCopy(img_out, img_result);
	cvResetImageROI(img_out);

	cvReleaseImage(&img_meanshift);
	cvReleaseImage(&img_in);
	cvReleaseImage(&img_out);

	return img_result;
};