#ifndef HOGDETECTION_H
#define HOGDETECTION_H

/*HogDetection算法实现
*@File:HogDetection.cpp
*@Author:Kan Chen (chenkan0007@gmail.com),
*@Last Modified:2013-06-16
*/

#define _CRT_NONSTDC_NO_WARNINGS

#include <math.h>
#include <tchar.h>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <set>
#include <vector>
#include "kmeans.h"
#include "meanshift.h"

#include "Classifier.h"
using namespace std;

#include "opencv\cv.h"
#include "opencv\highgui.h"

const double pi = 3.1416;
const int TOTAL_STAGE = 20; // number of classifier stages
const int HEIGHT = 64; // model height
const int WIDTH = 32; // model width
const int BKG_DETECTION_NUM_CLUSTER = 3;		// 背景提取时kmeans聚类步骤中设定的聚类数目
const int BKG_VIDEO_PIC_NUM_ALL = 50;				// 背景提取时选取的代表帧数目
const double THRES_DIFF_BINARY = 0.1 * 255;		// 背景剪除时提取连通域设定的阈值
const int EDGE_WIDTH = 60;									// 连通域的外围矩形与连通域空留的边长
const int GAUSSIAN_SMOOTH_R = 3;					// 进行高斯模糊时的半径
const float THRES_PERIMETER_RATIO = 20.0;		// 连通域分析时采用的周长与外围矩形周长比的阈值
const double THRES_AREA_ROI_MAX = 0.005;		// 第一轮筛选时检测框面积上限阈值
const double THRES_AREA_ROI_MIN = 0.001;		// 第一轮筛选时检测框面积下限阈值
const double THRES_DIFF_FRAME_BINARY = 0.03 * 255;	// 帧差法获取连通域时设定的阈值
const double THRES_JUD_RATIO = 0.75;				// 帧差法判定检测框中连通域与外围矩形的面积比阈值


struct rect
{
	int x1;
	int y1;
	int x2;
	int y2;
	int re;
	int count;
};

struct drawbox
{
	CvPoint point1;
	CvPoint point2;
	IplImage* image;
	IplImage* temp;
	bool isDraw;
};

// 第一轮筛选时判别是否为行人
bool judge_pedestrian(
	IplImage* frame, IplImage* frame_pre,
	CvRect roi, double thres_max, double thres_min,
	double thres_binary, double thres_ratio);

// 人工标定检测区域时的鼠标响应函数
void cvMouseCallback(int mouseEvent, int x, int y, int flags, void* param);

// 使用HOG分类器进行行人检测
void Detection(
	IplImage *img, IplImage *gray,
	double smin, double smax,
	double scalestep, int slidestep, int neighbor);

// 连通域分析函数
CvSeq* ConnectedComponents(cv::Mat &mask_process, int poly1_hull0, float perimScale, int number = 0,
	cv::Rect &bounding_box = cv::Rect(), cv::Point &contour_centers = cv::Point(-1, -1));


// 使用HOG分类器，背景剪除的行人检测(单幅图像)
void combo_DetectPicture(
	const TCHAR *pIn, const TCHAR *pOut,
	double smin, double smax,
	double scalestep, int slidestep, int neighbor, IplImage* background);

// 使用HOG分类器，背景剪除和帧差法的行人检测，返回已经标出行人的图像(单幅图像)
IplImage* combo_DetectPicture(
	IplImage* frame, IplImage* frame_pre, CvRect roi,
	double smin, double smax,
	double scalestep, int slidestep, int neighbor, IplImage* background);

// 使用HOG分类器，背景剪除的行人检测，返回已经标出行人的图像(单幅图像)
IplImage* combo_DetectPicture(
	IplImage* frame,
	double smin, double smax,
	double scalestep, int slidestep, int neighbor, IplImage* background);

// 使用HOG分类器，帧差法的行人检测，返回已经标出行人的图像(单幅图像)
IplImage* combo_DetectPicture_diff(
	IplImage* frame, IplImage* frame_pre,
	double smin, double smax, double thres_binary_frame,
	double scalestep, int slidestep, int neighbor);

// 使用背景剪除，帧差法的视频行人检测，返回中间人工标定所需的时间
double combo_DetectVideo(
	const TCHAR *pIn, const TCHAR *pOut, bool isdraw, bool isdiff,
	double smin, double smax, double scale,
	double scalestep, int slidestep, int neighbor, IplImage* bkg1);

// 单纯使用HOG分类器进行的行人检测(视频)
void DetectVideo(
	const TCHAR *pIn, const TCHAR *pOut, const TCHAR *pPolygon,
	double smin, double smax, double scale,
	double scalestep, int slidestep, int neighbor);

// 单纯使用HOG分类器进行的行人检测(单幅图像)
void DetectPicture(
	const TCHAR *pIn, const TCHAR *pOut, const TCHAR *pPolygon,
	double smin, double smax,
	double scalestep, int slidestep, int neighbor);

// 将图像数据转换为Kmeans算法执行时的数据结构
void readin_tuples(
	TCHAR pIn[], Tuple* &tuples, int pic_num_all, CvRect roi,
	int frameH, int frameW);

// 背景提取时对代表帧进行meanshift处理得到的图片，存在pOut所在的文件夹中
void create_meanshift_queue(const TCHAR *pIn,
	const TCHAR* pOut, int r, int pic_num_all);

// 背景提取函数，返回提取的背景图像
IplImage* video_bkg_detect(
	const TCHAR *pIn, const TCHAR* pTemp, int num_cluster, int r, int pic_num_all,
	int H_thres, int W_thres);

// 使用帧差法和HOG分类器的视频行人检测函数
void combo_DetectVideo_framediff(
	const TCHAR *pIn, const TCHAR *pOut,
	double smin, double smax, double thres_binary_frame,
	double scalestep, int slidestep, int neighbor, IplImage* bkg1);

// 获取帧差图像的连通域
CvSeq* get_framediff_components(
	IplImage* &frame, IplImage* frame_pre,
	double thres_binary_frame);

#endif