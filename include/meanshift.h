#ifndef MEAN_SHIFT_H
#define MEAN_SHIFT_H
#pragma once

/*meanshift算法实现
*@File:meanshift.cpp 
*@Author:Kan Chen (chenkan0007@gmail.com), 
*@Last Modified:2013-06-16 
*/ 

#include <iostream>
#include "opencv\cv.h"
#include <math.h>
#include "opencv\highgui.h"
#include <windows.h>

/*
#ifndef NDEBUG
#pragma comment(lib, "..\\lib\\opencv_core300d.lib")
#pragma comment(lib, "..\\lib\\opencv_highgui300d.lib")
#pragma comment(lib, "..\\lib\\opencv_imgproc300d.lib")
#else // NDEBUG
#pragma comment(lib, "..\\OpenCV\\lib\\opencv_core231.lib")
#pragma comment(lib, "..\\OpenCV\\lib\\opencv_highgui231.lib")
#pragma comment(lib, "..\\OpenCV\\lib\\opencv_imgproc231.lib")
#endif
*/

using namespace std;
//using namespace cv;

const int MEAN_SHIFT_R = 2;

// 将图像数据转换为meanshift算法执行时的数据点
IplImage* create_input4meanshift(IplImage* img, int r);

// 对每一个小区域进行meanshift聚类，返回中心处的数据
double meanshift(double* serArr, int p, int length);

// input image should be color image
IplImage* pic_meanshift(IplImage* img, int r);

#endif