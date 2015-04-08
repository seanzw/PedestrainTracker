#ifndef KMEANS_H
#define KMEANS_H
#pragma once

#include <iostream>  
#include <fstream>  
#include <vector>  
#include <math.h>  
#include "opencv\highgui.h"
#include "opencv\cv.h"

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

/*kmeans算法实现（考虑元组多个属性的情况） 
*@File:kmeans.cpp 
*@Author:Kan Chen (chenkan0007@gmail.com)
*@Create:2011-12-10 
*@Last Modified:2013-05-07 
*/ 

//const int NUM_clusters = 3;
const double THRES_Diff = 5.0e3;
const int THRES_ITERATION_TIMES = 150;

//存放元组的属性信息  
struct Tuple{  
	double attr1;  
	double attr2;  
	int num_elem;
	double* data;
};  

struct row_data
{
	double row;
	int label;
};

// 用于Kmeans中最后按照类别中元素数目从大到小排序的函数
bool lessmark(const row_data &s1, const row_data &s2);

// 用于Kmeans中最后按照类别中元素数目从小到大排序的函数
bool greatermark(const row_data &s1, const row_data &s2);

// 显示每个数据点信息的函数(用于调试)
void print_Tuple(Tuple tuple);

// 初始化每个数据点
void init_Tuple(Tuple &tuple, int num_element);

// 将图像数据转换为数据点
void readin_data(Tuple &tuple, int num, CvMat* &Matdata);

// 读取数据点的信息
void readout_data(Tuple &tuple, int frameH, int frameW);

// 删除数据点
void delete_tuple(Tuple &tuple);

//计算两个元组间的欧几里距离  
double getDistXY(Tuple t1, Tuple t2);

//根据质心，决定当前元组属于哪个簇  
int clusterOfTuple(Tuple means[],Tuple tuple, int num_cluster);

//获得给定簇集的平方误差  
double getVar(Tuple* &tuples,Tuple means[], int num_cluster, vector<int>* labels);

//获得当前簇的均值（质心）  
void cal_mean(Tuple* &cluster, Tuple & mean_tuple, vector<int> labels); 

//kMeans 算法实现
vector<int>* KMeans(Tuple* &tuples, int num_cluster, double thres_diff, int length);

#endif