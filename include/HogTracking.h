#ifndef HOGTRACKING_H
#define HOGTRACKING_H
#pragma once

/*HogTracking.h算法实现
*@File:HogTracking.cpp 
*@Author:Kan Chen (chenkan0007@gmail.com), 
*@Last Modified:2013-06-16 
*/ 

#include "HogDetection.h"

using namespace std;
//using namespace cv;

const double MERGEDISTANCE=32;
const int FRAMENUMBER=3;//跟踪的累积检测到的帧数
const int TIMEFRAME=5;//累积跟踪帧数
const int CAPACITY=30;//一帧中允许的检测到的目标的个数

struct part
{
	int cen_h;
	int cen_w;
	double width;
	double height;

	double predict_h;
	double predict_w;
	double predict_hv;
	double predict_wv;

	int state;
	//state=0: 该点没有用到
	//state=1: 刚检测的点，等待判断(预热态)
	//state=2: 显示的点，且是受到关注的点
	//state=3: 显示的点，且正在走下坡路

	int state_count;
	int seriesnumber;
	int tracknumber;

	int flag;
	int flagcircle;
};

struct info
{
	part point[CAPACITY];
};

class model
{
public:
	CvFont font;
	double objectInfo[3];
	info dot[TIMEFRAME];
	info temdot;

	feature *classifier[TOTAL_STAGE];
	int numweak[TOTAL_STAGE];
	integral **source;
	integral **s;

	CvMat *polygon;
	CvRect roi;
	CvPoint crosslinept1;
	CvPoint crosslinept2;

	std::set<int> front;
	std::set<int> back;
};

struct output
{
	int front;
	int back;
	int num;
	CvPoint position[30];
};

// 跟踪算法中的检测函数，返回rect_seq的矩形序列，包含可能有行人的矩形区域，被Detection_t调用
void Detection(
	IplImage *img, IplImage *gray, CvRect roi_cur,
	feature *classifier[], int numweak[],
	integral **source, integral **s, CvSeq* &rect_seq,
	double smin, double smax,
	double scalestep, int slidestep, int neighbor);

// 跟踪与检测函数，被combo_Video_Tracking函数调用
output DetectionTracking(
	IplImage* frame, IplImage* frame_pre, CvRect roi0,
	feature *classifier[], int numweak[],
	double smin, double smax,
	double scalestep, int slidestep, int neighbor, IplImage* bkg1, model &md);

// 单幅图像的行人检测，返回pedestrians序列，被Detection函数调用
void combo_DetectPicture_t(
	IplImage* frame, IplImage* frame_pre, 
	CvSeq* &pedestrians, CvRect roi0,
	feature *classifier[], int numweak[],
	double smin, double smax,
	double scalestep, int slidestep, int neighbor, IplImage* bkg1);

// 检测行人的函数，被DetectionTracking函数调用
info Detection_t(
	IplImage* frame, IplImage* frame_pre, CvRect roi0,
	feature *classifier[], int numweak[],
	double smin, double smax,
	double scalestep, int slidestep, int neighbor, IplImage* bkg1, model &md);

// 视频行人跟踪函数
void combo_Video_Tracking(
	const char *pIn, const char *pOut, const char* pPolygon,
	bool isdiff, feature *classifier[], int numweak[],
	double smin, double smax, double scale,
	double scalestep, int slidestep, int neighbor, IplImage* bkg1);

// 判断行人状态
void JudgeState(model &md);

// 绘制跟踪的行人
void DrawTarget(IplImage *img, model &md, output &result);

// 更新预测信息
void UpdatePrediction(info dot[]);

#endif