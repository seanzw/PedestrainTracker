#include <iostream>
//#include <opencv2\opencv.hpp>
#include <fstream>
#include <string>
#include "HogDetection.h"

using namespace cv;
using namespace std;

//Just some convienience macros
#define CV_CVX_WHITE    CV_RGB(0xff,0xff,0xff)
#define CV_CVX_BLACK    CV_RGB(0x00,0x00,0x00)


CvSeq* ConnectedComponents(Mat &mask_process, int poly1_hull0, float perimScale, int number,
	Rect &bounding_box, Point &contour_centers)
{
	IplImage *mask = &IplImage(mask_process);
	int *num = &number;
	CvRect *bbs = &CvRect(bounding_box);
	CvPoint *centers = &CvPoint(contour_centers);
	static CvMemStorage*    mem_storage    = NULL;
	static CvSeq*            contours    = NULL;
	//CLEAN UP RAW MASK
	//开运算作用：平滑轮廓，去掉细节,断开缺口
	cvMorphologyEx( mask, mask, NULL, NULL, CV_MOP_OPEN, 1 );//对输入mask进行开操作，CVCLOSE_ITR为开操作的次数，输出为mask图像
	
	//cvNamedWindow("openmask");
	//cvShowImage("openmask",mask);


	//闭运算作用：平滑轮廓，连接缺口
	cvMorphologyEx( mask, mask, NULL, NULL, CV_MOP_CLOSE, 1 );//对输入mask进行闭操作，CVCLOSE_ITR为闭操作的次数，输出为mask图像
	
	//cvNamedWindow("closemask");
	//cvShowImage("closemask",mask);
	//cvWaitKey(0);
	//cvDestroyWindow("openmask");
	//cvDestroyWindow("closemask");

	//FIND CONTOURS AROUND ONLY BIGGER REGIONS
	if( mem_storage==NULL ) mem_storage = cvCreateMemStorage(0);
	else cvClearMemStorage(mem_storage);
	//CV_RETR_EXTERNAL=0是在types_c.h中定义的，CV_CHAIN_APPROX_SIMPLE=2也是在该文件中定义的
	CvContourScanner scanner = cvStartFindContours(mask,mem_storage,sizeof(CvContour),CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);
	CvSeq* c;
	int numCont = 0;
	//该while内部只针对比较大的轮廓曲线进行替换处理
	while( (c = cvFindNextContour( scanner )) != NULL )
	{
		//double len = cvContourPerimeter( c );
		double len = cvContourArea(c);
		CvRect roi_temp = cvBoundingRect(c);
		double q = ((roi_temp.height+90) * (roi_temp.width+90)) / perimScale;
		//double q = (roi_temp.height + roi_temp.width) /perimScale;   //calculate perimeter len threshold
		if( len < q ) //Get rid of blob if it's perimeter is too small
		{
			cvSubstituteContour( scanner, NULL );    //用NULL代替原来的那个轮廓
		}
		else //Smooth it's edges if it's large enough
		{
			CvSeq* c_new;
			if(poly1_hull0) //Polygonal approximation of the segmentation
				c_new = cvApproxPoly(c,sizeof(CvContour),mem_storage,CV_POLY_APPROX_DP, 2,0);
			else //Convex Hull of the segmentation
				c_new = cvConvexHull2(c,mem_storage,CV_CLOCKWISE,1);
			cvSubstituteContour( scanner, c_new ); //最开始的轮廓用凸包或者多项式拟合曲线替换
			numCont++;
		}
	}
	contours = cvEndFindContours( &scanner );    //结束轮廓查找操作
	// PAINT THE FOUND REGIONS BACK INTO THE IMAGE
	cvZero( mask );
	IplImage *maskTemp;
	//CALC CENTER OF MASS AND OR BOUNDING RECTANGLES
	if(*num != 0)
	{
		int N = *num, numFilled = 0, i=0;
		CvMoments moments;
		double M00, M01, M10;
		maskTemp = cvCloneImage(mask);
		for(i=0, c=contours; c != NULL; c = c->h_next,i++ )        //h_next为轮廓序列中的下一个轮廓
		{
			if(i < N) //Only process up to *num of them
			{
				//CV_CVX_WHITE在本程序中是白色的意思
				cvDrawContours(maskTemp,c,CV_CVX_WHITE, CV_CVX_WHITE,-1,CV_FILLED,8);
				//Find the center of each contour
				if(centers != &cvPoint(-1, -1))
				{
					cvMoments(maskTemp,&moments,1);    //计算mask图像的最高达3阶的矩
					M00 = cvGetSpatialMoment(&moments,0,0); //提取x的0次和y的0次矩
					M10 = cvGetSpatialMoment(&moments,1,0); //提取x的1次和y的0次矩
					M01 = cvGetSpatialMoment(&moments,0,1); //提取x的0次和y的1次矩
					centers[i].x = (int)(M10/M00);    //利用矩的结果求出轮廓的中心点坐标
					centers[i].y = (int)(M01/M00);
				}
				//Bounding rectangles around blobs
				if(bbs != &CvRect())
				{
					bbs[i] = cvBoundingRect(c); //算出轮廓c的外接矩形
				}
				cvZero(maskTemp);
				numFilled++;
			}
			//Draw filled contours into mask
			cvDrawContours(mask,c,CV_CVX_WHITE,CV_CVX_WHITE,-1,CV_FILLED,8); //draw to central mask
		} //end looping over contours
		*num = numFilled;
		cvReleaseImage( &maskTemp);
	}
	//ELSE JUST DRAW PROCESSED CONTOURS INTO THE MASK
	else
	{
		for( c=contours; c != NULL; c = c->h_next )
		{
			cvDrawContours(mask,c,CV_CVX_WHITE, CV_CVX_BLACK,-1,CV_FILLED,8);
		}
	}
	return contours;
}