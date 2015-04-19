/*******************************************
 Header for whole system.
 *******************************************/

#ifndef PEDESTRAIN_COUNTING_HEADER
#define PEDESTRAIN_COUNTING_HEADER

#include "kmeans.h"
#include "meanshift.h"

#include "VideoDetector.h"
#include "BKGCutDetector.h"
#include "AdaBoostClassifier.h"
#include "HoGIntegralImage.h"


const int BKG_DETECTION_NUM_CLUSTER = 3;
const int BKG_VIDEO_PIC_NUM_ALL = 50;

#define HEIGHT 64
#define WIDTH 32

IplImage* video_bkg_detect(
	const TCHAR *pIn, const TCHAR* pTemp, int num_cluster, int r, int pic_num_all,
	int H_thres, int W_thres);

void create_meanshift_queue(const TCHAR *pIn,
	const TCHAR* pOut, int r, int pic_num_all);

void readin_tuples(
	TCHAR pIn[], Tuple* &tuples, int pic_num_all, CvRect roi,
	int frameH, int frameW);

#endif