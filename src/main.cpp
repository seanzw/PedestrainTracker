#include <iostream>
#include <windows.h>
#include <stdio.h>

#include "PedestrainCounting.h"

using namespace std;

void GetTarget(int event, int x, int y, int flags, void *userParams);

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
	opt.maxAreaRatio = 0.05f;
	opt.minAreaRatio = 0.001f;

	AdaBoostClassifier classi("..\\HOGClassifier\\");


	//get the time of running program
	LARGE_INTEGER freq;
	LARGE_INTEGER start_t, stop_t;
	double exe_time = 0.0;
	QueryPerformanceFrequency(&freq);

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

	if (!strcmp(argv[1], "-p")) {
		QueryPerformanceCounter(&start_t);
		
		// read image
		cv::Mat frame = cv::imread(argv[2]);
		cv::Mat background = cv::imread(argv[3], CV_LOAD_IMAGE_GRAYSCALE);

		// read designated polygon
		cv::Rect roi(0, 0, frame.size().width, frame.size().height);

		cv::Mat img = frame(roi);
		cv::Mat gray(img.size(), CV_8UC1);
		cv::cvtColor(img, gray, cv::COLOR_RGB2GRAY);
		
		HoGIntegralImage intImage(gray);
		BKGCutDetector detector(&intImage, &classi, opt);

		detector.Detect(gray, cv::Point(0, 0), true, background);
		detector.DrawDetection(img);

		cv::imwrite(argv[4], frame);

		// release data buffer
		frame.release();
		img.release();
		gray.release();
		

		QueryPerformanceCounter(&stop_t);
		exe_time = double(stop_t.QuadPart - start_t.QuadPart) / freq.QuadPart;
		printf("The program executed time is %fs.\n", exe_time);
	}

	if (!strcmp(argv[1], "-phog")) {
		QueryPerformanceCounter(&start_t);
		
		// read image
		cv::Mat frame = cv::imread(argv[2]);

		// read designated polygon
		cv::Rect roi(0, 0, frame.size().width, frame.size().height);

		cv::Mat img = frame(roi);
		cv::Mat gray(img.size(), CV_8UC1);
		cv::cvtColor(img, gray, cv::COLOR_RGB2GRAY);

		HoGIntegralImage intImage(gray);
		ImageDetector detector(&intImage, &classi, opt);

		detector.Detect(gray, cv::Point(0, 0), true);
		detector.DrawDetection(img);

		cv::imwrite(argv[3], frame);

		// release data buffer
		frame.release();
		img.release();
		gray.release();

		QueryPerformanceCounter(&stop_t);
		exe_time = double(stop_t.QuadPart - start_t.QuadPart) / freq.QuadPart;
		printf("The program executed time is %fs.\n", exe_time);
	}

	if (!strcmp(argv[1], "-vhog"))
	{
		QueryPerformanceCounter(&start_t);

		// Open the video.
		cv::VideoCapture in(argv[2]);
		if (!in.isOpened()) {
			printf("Can't open video: %s\n", argv[2]);
		}

		// Get the basic inforamtion.
		double fps = in.get(cv::CAP_PROP_FPS);
		int width = (int)in.get(cv::CAP_PROP_FRAME_WIDTH);
		int height = (int)in.get(cv::CAP_PROP_FRAME_HEIGHT);
		int ex = (int)in.get(cv::CAP_PROP_FOURCC);
		
		// Open the out video.
		cv::VideoWriter out(argv[3], ex, fps / 2.0f, cv::Size(width, height));

		// Initialize the detector.
		HoGIntegralImage intImage(width, height);
		ImageDetector imageDetector(&intImage, &classi, opt);
		VideoDetector videoDetector(&imageDetector, opt);

		// Do the detection.
		videoDetector.Detect(in, out);

		QueryPerformanceCounter(&stop_t);
		exe_time = double(stop_t.QuadPart - start_t.QuadPart) / freq.QuadPart;
		printf("The program executed time is %fs.\n", exe_time);
	}

	if (!strcmp(argv[1], "-v")) {

		QueryPerformanceCounter(&start_t);

		// Read the background.
		cv::Mat bkg = cv::imread(argv[4], CV_LOAD_IMAGE_GRAYSCALE);

		// Open the video.
		cv::VideoCapture in(argv[2]);
		if (!in.isOpened()) {
			printf("Can't open video: %s\n", argv[2]);
		}

		// Get the basic inforamtion.
		double fps = in.get(cv::CAP_PROP_FPS);
		int width = (int)in.get(cv::CAP_PROP_FRAME_WIDTH);
		int height = (int)in.get(cv::CAP_PROP_FRAME_HEIGHT);
		int ex = (int)in.get(cv::CAP_PROP_FOURCC);

		// Open the out video.
		cv::VideoWriter out(argv[3], ex, fps / 2.0f, cv::Size(width, height));

		// Initialize the detector.
		HoGIntegralImage intImage(width, height);
		BKGCutDetector detector(&intImage, &classi, opt);
		VideoDetector videoDetector(&detector, opt);

		// Do the detection.
		videoDetector.Detect(in, out, bkg);

		QueryPerformanceCounter(&stop_t);

		exe_time = double(stop_t.QuadPart - start_t.QuadPart) / freq.QuadPart;
		printf("===================\n");
		printf("The program executed time is %fs.\n", exe_time);

	}

	if (!strcmp(argv[1], "--particle-tracker")) {

		cv::VideoCapture in(argv[2]);
		if (!in.isOpened()) {
			printf("Can't open video: %s\n", argv[2]);
		}

		// Get the basic inforamtion.
		double fps = in.get(cv::CAP_PROP_FPS);
		int width = (int)in.get(cv::CAP_PROP_FRAME_WIDTH);
		int height = (int)in.get(cv::CAP_PROP_FRAME_HEIGHT);
		int ex = (int)in.get(cv::CAP_PROP_FOURCC);

		// Open the out video.
		cv::VideoWriter out(argv[3], ex, fps / 2.0f, cv::Size(width, height));

		// Show the first frame and get the roi.
		Rect target;
		cv::Mat first(width, height, CV_8UC3);
		in.read(first);
		cv::imshow("First Frame", first);
		cv::setMouseCallback("First Frame", GetTarget, (void *)&target);
		cv::waitKey(0);
		cv::rectangle(first, (cv::Rect)target, cv::Scalar(255.0f), 2);
		cv::imshow("First Frame", first);
		cv::waitKey(0);
		in.set(cv::CAP_PROP_POS_FRAMES, 0);

		GrayScaleIntegralImage grayIntImage(width, height);
		StrongClassifierDirectSelect classifier(50, 250, Size(target.width, target.height), 2);
		ParticleFilter particleFilter(&classifier, &grayIntImage, target, 100);

		ParticleFilterTracker pfTracker(&classifier, &grayIntImage, &particleFilter);

		QueryPerformanceCounter(&start_t);

		pfTracker.Track(in, out);

		QueryPerformanceCounter(&stop_t);
		exe_time = double(stop_t.QuadPart - start_t.QuadPart) / freq.QuadPart;
		printf("===================\n");
		printf("The program executed time is %fs.\n", exe_time);

	}

	return 0;
}

void GetTarget(int event, int x, int y, int flags, void *userParam) {
	if (event == CV_EVENT_LBUTTONDOWN) {
		Rect *roi = (Rect *)userParam;
		roi->upper = y;
		roi->left = x;
	}
	if (event == CV_EVENT_LBUTTONUP) {
		Rect *roi = (Rect *)userParam;
		roi->width = x - roi->left;
		roi->height = y - roi->upper;
	}
}