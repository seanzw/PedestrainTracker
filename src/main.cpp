#include <iostream>
#include <windows.h>
#include <stdio.h>

#include "PedestrainCounting.h"

using namespace std;

int main(int argc, char *argv[]) {
	char *pPolygon = NULL;
	char *bkgpath = NULL;
	float smin = 1.5f;
	float smax = 2.5f;
	float scalestep = 1.07f;
	int slidestep = 2;
	int neighbor = 8;
	bool isdraw = false;
	bool isdiff = false;
	float scale_v = 1.0;

	if (strcmp(argv[1], "-v") || strcmp(argv[1], "-t"))
	{
		if (argc > 4) pPolygon = argv[4];
		if (argc > 6) smin = (float)atof(argv[5]);
		if (argc > 6) smax = (float)atof(argv[6]);
		if (argc > 7) scalestep = (float)atof(argv[7]);
		if (argc > 8) slidestep = atoi(argv[8]);
		if (argc > 9) neighbor = atoi(argv[9]);
	}
	else
	{
		if (!strcmp(argv[1], "-v"))
		{
			if (argc > 5) { isdraw = (!strcmp(argv[5], "y")); }
			if (argc > 6) { isdiff = (!strcmp(argv[6], "y")); }
			if (argc > 7) { scale_v = (float)(float)atof(argv[7]); }
			if (argc > 9) smin = (float)atof(argv[8]);
			if (argc > 9) smax = (float)atof(argv[9]);
			if (argc > 10) scalestep = (float)atof(argv[10]);
			if (argc > 11) slidestep = atoi(argv[11]);
			if (argc > 12) neighbor = atoi(argv[12]);
		}
		else
		{
			if (argc > 5) bkgpath = argv[5];
			if (argc > 7) smin = (float)atof(argv[6]);
			if (argc > 7) smax = (float)atof(argv[7]);
			if (argc > 8) scalestep = (float)atof(argv[8]);
			if (argc > 9) slidestep = atoi(argv[9]);
			if (argc > 10) neighbor = atoi(argv[10]);
		}
	}

	Options opt;
	opt.scaleMin = smin;
	opt.scaleMax = smax;
	opt.scaleStep = scalestep;
	opt.slideStep = slidestep;
	opt.evidence = neighbor;
	opt.modelHeight = HEIGHT;
	opt.modelWidth = WIDTH;
	opt.binaryThre = 25.5f;
	opt.invPerimeterRatio = 0.05f;
	opt.maxAreaRatio = 0.005f;
	opt.minAreaRatio = 0.001f;

	opt.detectionWeight = 1.0f;
	opt.distWeight = 0.5f;
	opt.initVelocity = Point2D(0, 0);
	opt.matchThre = 5.0f;
	opt.nParticles = 200;
	opt.numBackups = 5;
	opt.numSelectors = 50;
	opt.numWeakClassifiers = 250;
	opt.target = Rect(0, 0, 64, 128);
	opt.targetsFreeListCapacity = 20;
	opt.velocitySigmaConst = 10.0f;
	opt.velocityThre = 10.0f;


	//get the time of running program
	LARGE_INTEGER freq;
	LARGE_INTEGER start_t, stop_t;
	double exe_time = 0.0;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&start_t);

	if (!strcmp(argv[1], "-h")) {
		PrintHelp();
	}

	if (!strcmp(argv[1], "-b")) {
		QueryPerformanceCounter(&start_t);
		Tuple* means = NULL;

		CvCapture *capture = cvCaptureFromAVI(argv[2]);
		int frameW = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
		int frameH = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
		cvReleaseCapture(&capture);

		IplImage* background = cvCreateImage(cvSize(frameW, frameH), IPL_DEPTH_8U, 1);

		background = video_bkg_detect(argv[2], argv[3], BKG_DETECTION_NUM_CLUSTER, MEAN_SHIFT_R, BKG_VIDEO_PIC_NUM_ALL, frameH, frameW);
		cvSaveImage(argv[4], background);
	}

	if (!strcmp(argv[1], "-phog")) {
		DetectSinglePictureHOG(argv[2], argv[3], adaboostFile, opt);
	}

	if (!strcmp(argv[1], "-p")) {
		DetectSinglePictureBKG(argv[2], argv[3], argv[4], adaboostFile, opt);
	}

	if (!strcmp(argv[1], "-vhog")) {
		DetectVideoHOG(argv[2], argv[3], adaboostFile, opt);
	}

	if (!strcmp(argv[1], "-v")) {
		DetectVideoBKG(argv[2], argv[3], argv[4], adaboostFile, opt);
	}

	if (!strcmp(argv[1], "--particle-tracker")) {
		TrackVideoSingle(argv[2], argv[3]);
	}

	if (!strcmp(argv[1], "--multiple-tracker")) {
		TrackVideoMulti(argv[2], argv[3], argv[4], adaboostFile, opt);
	}

	QueryPerformanceCounter(&stop_t);
	exe_time = double(stop_t.QuadPart - start_t.QuadPart) / freq.QuadPart;
	fprintf(stdout, "The program executed time is %fs.\n", exe_time);

	return 0;
}

