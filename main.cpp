#include <iostream>
#include <windows.h>
#include <stdio.h>
#include "HogDetection.h"
#include "HogTracking.h"

using namespace std;

void main(int argc, char *argv[])
{
	char *pPolygon = NULL;
	char *bkgpath = NULL;
	double smin = 1.5;
	double smax = 2.5;
	double scalestep = 1.07;
	int slidestep = 2;
	int neighbor = 8;
	bool isdraw = false;
	bool isdiff = false;
	float scale_v = 1.0;

	//if(argc<2)
	//{
	//	cout<<"Usage: HogDetection.exe -p|-v inFile outFile [polygonFile] [sMin sMax] [scaleStep] [sildeStep] [neighbor]"<<endl;
	//	return;
	//}
	//else if(strcmp(argv[1],"-p") && strcmp(argv[1],"-v") && strcmp(argv[1],"-vhog") && strcmp(argv[1],"-phog"))
	//{
	//	cout<<"Usage: HogDetection.exe -p|-v inFile outFile [polygonFile] [sMin sMax] [scaleStep] [sildeStep] [neighbor]"<<endl;
	//	return;
	//}
	//else if(argc<4)
	//{
	//	cout<<"Usage: HogDetection.exe -p|-v inFile outFile [polygonFile] [sMin sMax] [scaleStep] [sildeStep] [neighbor]"<<endl;
	//	return;
	//}

	if (strcmp(argv[1], "-v") || strcmp(argv[1], "-t"))
	{
		if (argc > 4) pPolygon = argv[4];
		if (argc > 6) smin = atof(argv[5]);
		if (argc > 6) smax = atof(argv[6]);
		if (argc > 7) scalestep = atof(argv[7]);
		if (argc > 8) slidestep = atoi(argv[8]);
		if (argc > 9) neighbor = atoi(argv[9]);
	}
	else
	{
		if (!strcmp(argv[1], "-v"))
		{
			if (argc > 5) { isdraw = (!strcmp(argv[5], "y")); }
			if (argc > 6) { isdiff = (!strcmp(argv[6], "y")); }
			if (argc > 7) { scale_v = atof(argv[7]); }
			if (argc > 9) smin = atof(argv[8]);
			if (argc > 9) smax = atof(argv[9]);
			if (argc > 10) scalestep = atof(argv[10]);
			if (argc > 11) slidestep = atoi(argv[11]);
			if (argc > 12) neighbor = atoi(argv[12]);
		}
		else
		{
			if (argc > 5) bkgpath = argv[5];
			if (argc > 7) smin = atof(argv[6]);
			if (argc > 7) smax = atof(argv[7]);
			if (argc > 8) scalestep = atof(argv[8]);
			if (argc > 9) slidestep = atoi(argv[9]);
			if (argc > 10) neighbor = atoi(argv[10]);
		}
	}

	// read classifier
	feature *classifier[TOTAL_STAGE];
	int numweak[TOTAL_STAGE];
	ReadClassifier(classifier, numweak);

	//get the time of running program
	LARGE_INTEGER freq;
	LARGE_INTEGER start_t, stop_t;
	double exe_time = 0.0;
	QueryPerformanceFrequency(&freq);

	if (strcmp(argv[1], "-b") && strcmp(argv[1], "-p") && strcmp(argv[1], "-v") && strcmp(argv[1], "-phog")
		&& strcmp(argv[1], "-vhog") && strcmp(argv[1], "-h") && strcmp(argv[1], "-t"))
	{
		cout << "Please type HogDetection.exe -h for more help" << endl;
	}

	if (!strcmp(argv[1], "-h"))
	{
		cout << "Extract background image from video" << endl;
		cout << "HogDetection.exe -b inFile tempFile outFile" << endl;
		cout << "  inFile: The path of video waiting to be detected" << endl;
		cout << "  tempFile: The directory of saving temp pictures during extracting(default = E:\\Human_detection\\result\\temp_meanshift\\)" << endl;
		cout << "  outFile: The path of result to be saved" << endl << endl;

		cout << "Detect Pedestrians in single picture using new method" << endl;
		cout << "Please FIRST extract the background images from video before detect pedestrians using new method" << endl;
		cout << "HogDetection.exe -p inFile bkg outFile" << endl;
		cout << "  inFile: The path of video waiting to be detected" << endl;
		cout << "  outFile: The path of result to be saved" << endl;
		cout << "  bkg: The path of background picture extracted from video" << endl << endl;

		cout << "Detect Pedestrians in video using new method:" << endl;
		cout << "Please FIRST extract the background images from video before detect pedestrians using new method" << endl;
		cout << "HogDetection.exe -v inFile outFile bkg isdraw isdiff scaleVideo smin smax scalestep slidestep neighbor" << endl;
		cout << "  inFile: The path of video waiting to be detected" << endl;
		cout << "  outFile: The path of result to be saved" << endl;
		cout << "  bkg: The path of background picture extracted from video" << endl;
		cout << "  isdraw: Whether to draw ROI in video (full screen scan by default)" << endl;
		cout << "  scaleVideo: The scale of video size (default = 1.0)" << endl;
		cout << "  isdiff: Whether to apply difference between frames analysis during detection (false by default)" << endl;
		cout << "  smin: Minimum scale of picture scale during detection process (default = 1.5)" << endl;
		cout << "  smax: Maximum scale of picture scale during detection process (default = 2.5)" << endl;
		cout << "  scalestep: The step of scale increment (default = 1.07)" << endl;
		cout << "  slidestep: The step of detection window (default = 2)" << endl;
		cout << "  neighbor: Minimum scan times of detection (default = 8)" << endl << endl;

		cout << "Detect Pedestrians in picture/video using old method" << endl;
		cout << "HogDetection.exe -vhog|-phog inFile outFile [polygonFile [sMin sMax [scaleStep [sildeStep [neighbor]]]]]" << endl;
		cout << "  inFile: The path of video waiting to be detected" << endl;
		cout << "  outFile: The path of result to be saved" << endl;
		cout << "  PolygonFile: The path of file saving polygon's information" << endl;
		cout << "  smin: Minimum scale of picture scale during detection process (default = 1.5)" << endl;
		cout << "  smax: Maximum scale of picture scale during detection process (default = 2.5)" << endl;
		cout << "  scalestep: The step of scale increment (default = 1.07)" << endl;
		cout << "  slidestep: The step of detection window (default = 2)" << endl;
		cout << "  neighbor: Minimum scan times of detection (default = 8)" << endl;
	}

	if (!strcmp(argv[1], "-b"))
	{
		QueryPerformanceCounter(&start_t);
		//int num_cluster = 5;
		//int pic_num_all = 100;
		Tuple* means = NULL;

		CvCapture *capture = cvCaptureFromAVI(argv[2]);
		int frameW = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
		int frameH = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
		cvReleaseCapture(&capture);

		IplImage* background = cvCreateImage(cvSize(frameW, frameH), IPL_DEPTH_8U, 1);

		background = video_bkg_detect(argv[2], argv[3], BKG_DETECTION_NUM_CLUSTER, MEAN_SHIFT_R, BKG_VIDEO_PIC_NUM_ALL, frameH, frameW);
		cvSaveImage(argv[4], background);

		QueryPerformanceCounter(&stop_t);
		exe_time = double(stop_t.QuadPart - start_t.QuadPart) / freq.QuadPart;
		fprintf(stdout, "The program executed time is %fs.\n", exe_time);

	}

	if (!strcmp(argv[1], "-p"))
	{
		cout << argv[3] << endl;
		QueryPerformanceCounter(&start_t);

		IplImage* background = cvLoadImage(argv[3], 0);
		IplImage* frame = cvLoadImage(argv[2]);
		frame = combo_DetectPicture(frame, classifier, numweak, smin, smax, scalestep, slidestep, neighbor, background);
		QueryPerformanceCounter(&stop_t);

		cvSaveImage(argv[4], frame);
		exe_time = double(stop_t.QuadPart - start_t.QuadPart) / freq.QuadPart;
		fprintf(stdout, "The program executed time is %fs.\n", exe_time);
	}

	if (!strcmp(argv[1], "-phog"))
	{
		QueryPerformanceCounter(&start_t);

		DetectPicture(argv[2], argv[3], pPolygon, classifier, numweak, smin, smax, scalestep, slidestep, neighbor);
		QueryPerformanceCounter(&stop_t);
		exe_time = double(stop_t.QuadPart - start_t.QuadPart) / freq.QuadPart;
		fprintf(stdout, "The program executed time is %fs.\n", exe_time);
	}

	if (!strcmp(argv[1], "-vhog"))
	{
		QueryPerformanceCounter(&start_t);

		double scale_v = 1.0;
		DetectVideo(argv[2], argv[3], pPolygon, classifier, numweak, smin, smax, scale_v, scalestep, slidestep, neighbor);
		QueryPerformanceCounter(&stop_t);
		exe_time = double(stop_t.QuadPart - start_t.QuadPart) / freq.QuadPart;
		fprintf(stdout, "The program executed time is %fs.\n", exe_time);
	}

	if (!strcmp(argv[1], "-t"))
	{
		QueryPerformanceCounter(&start_t);
		double scale_v = 1.0;
		IplImage* background = cvLoadImage(argv[5], 0);
		combo_Video_Tracking(argv[2], argv[3], argv[4], false, classifier, numweak, smin, smax, scale_v, scalestep, slidestep, neighbor, background);
		QueryPerformanceCounter(&stop_t);
		exe_time = double(stop_t.QuadPart - start_t.QuadPart) / freq.QuadPart;
		fprintf(stdout, "The program executed time is %fs.\n", exe_time);
	}

	if (!strcmp(argv[1], "-v"))
	{

		QueryPerformanceCounter(&start_t);

		IplImage* background = cvLoadImage(argv[4], 0);
		cout << (int)background << endl;
		double exe_time2;
		char* res_draw;
		char* res_diff;
		if (isdraw){ res_draw = "true"; }
		else{ res_draw = "false"; }
		if (isdiff){ res_diff = "true"; }
		else{ res_diff = "false"; }
		cout << "isdraw = " << res_draw << endl;
		cout << "isdiff = " << res_diff << endl;
		cout << "scale_v = " << scale_v << endl;
		cout << "smin = " << smin << endl;
		cout << "smax = " << smax << endl;
		system("pause");

		exe_time2 = combo_DetectVideo(argv[2], 
			argv[3], 
			isdraw, 
			isdiff, 
			classifier, 
			numweak, 
			smin, 
			smax, 
			scale_v, 
			scalestep, 
			slidestep, 
			neighbor, 
			background
			);
		QueryPerformanceCounter(&stop_t);
		exe_time = double(stop_t.QuadPart - start_t.QuadPart) / freq.QuadPart;
		cout << "===================" << endl;
		fprintf(stdout, "The program executed time is %fs.\n", exe_time - exe_time2);

		cvReleaseImage(&background);
	}

	// release classifier
	for (int i = 0; i < TOTAL_STAGE; i++)
		delete[] classifier[i];

	//system("pause");


}