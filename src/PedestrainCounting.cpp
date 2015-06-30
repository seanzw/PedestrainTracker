#include "PedestrainCounting.h"

PedestrainCounter::PedestrainCounter() {
    opt.scaleMin = 1.5f;
    opt.scaleMax = 2.5f;
    opt.scaleStep = 1.07f;
    opt.slideStep = 2;
    opt.evidence = 8;
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
    opt.nParticles = 500;
    opt.numBackups = 5;
    opt.numSelectors = 50;
    opt.numWeakClassifiers = 250;
    opt.target = Rect(0, 0, 64, 128);
    opt.targetsFreeListCapacity = 20;
    opt.velocitySigmaConst = 10.0f;
    opt.velocityThre = 10.0f;
}

void PedestrainCounter::ParseParams(int argc, char *argv[]) {

    if (!strcmp(argv[1], "-h")) {
        PrintHelp();
    }

    if (!strcmp(argv[1], "-b")) {
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
    else {
        PrintHelp();
    }

}

IplImage* PedestrainCounter::video_bkg_detect(
	const TCHAR *pIn, const TCHAR* pTemp, int num_cluster, int r, int pic_num_all,
	int H_thres, int W_thres) {
	CvCapture *capture = cvCaptureFromAVI(pIn);
	int frameW = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
	int frameH = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
	cvReleaseCapture(&capture);

	// frameH = 480; frameW = 640;
	IplImage* background = cvCreateImage(cvSize(frameW, frameH), IPL_DEPTH_8U, 1);

	int h = frameH; int w = frameW;
	int countH = 0; int countW = 0;
	while (h > H_thres)
	{
		h = (int)floor(h / 2);
		countH++;
	}
	while (w > W_thres)
	{
		w = (int)floor(w / 2);
		countW++;
	}

	IplImage* bkg_temp = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 1);
	int bkg_step = bkg_temp->widthStep;
	uchar* bkg_data = (uchar*)bkg_temp->imageData;

	cout << "Processing patch's size = (" << h << "," << w << ")" << endl;

	// Get the images after mean-shift.
	TCHAR meanshift_path[256];
	if (pTemp == NULL)
	{
		sprintf_s(meanshift_path, TEXT("E:\\Human_detection\\result\\temp_meanshift\\"));
	}
	else
	{
		sprintf_s(meanshift_path, pTemp);
	}

	int flag = CreateDirectory(meanshift_path, NULL);
	create_meanshift_queue(pIn, meanshift_path, r, pic_num_all);

	Tuple* tuples = new Tuple[pic_num_all];
	Tuple* means = new Tuple[num_cluster];

	for (int i = 0; i < pic_num_all; i++)
	{
		init_Tuple(tuples[i], h*w);
	}
	for (int i = 0; i < num_cluster; i++)
	{
		init_Tuple(means[i], h*w);
	}
	for (int hh = 0; hh < pow(2, countH); hh++)
	{
		for (int ww = 0; ww < pow(2, countW); ww++)
		{
			// Read in elements.
			cout << "Processing patch (" << hh << "," << ww << ")" << endl;

			for (int i = 0; i < pic_num_all; i++)
			{
				delete_tuple(tuples[i]);
				init_Tuple(tuples[i], h*w);
			}
			CvRect roi = cvRect(ww*w, hh*h, w, h);
			readin_tuples(meanshift_path, tuples, pic_num_all, roi, frameH, frameW);

			// k-means
			vector<int> *labels = new vector<int>[num_cluster];
			labels = KMeans(tuples, num_cluster, THRES_Diff, pic_num_all);

			row_data row_current;
			vector<row_data> row_ordered;
			for (int i = 0; i < num_cluster; i++)
			{
				delete_tuple(means[i]);
				init_Tuple(means[i], h*w);
				cal_mean(tuples, means[i], labels[i]);
				row_current.row = labels[i].size();
				row_current.label = i;
				row_ordered.push_back(row_current);
			}

			sort(row_ordered.begin(), row_ordered.end(), greatermark);

			cout << "The most probable background is cluster " << row_ordered[0].label << endl;

			// Get local background and copy it.
			for (int ii = 0; ii < h; ii++)
			{
				for (int jj = 0; jj < w; jj++)
				{
					bkg_data[ii*bkg_step + jj] = max(0, min(255, (int)means[row_ordered[0].label].data[ii*w + jj]));
				}
			}

			cvSetImageROI(background, roi);
			cvCopy(bkg_temp, background);
			cvResetImageROI(background);

		}
	}

	// Release memory.
	TCHAR order[300];
	for (int ii = 0; ii < num_cluster; ii++)
	{
		delete_tuple(means[ii]);
	}
	delete[]means;
	for (int ii = 0; ii < pic_num_all; ii++)
	{
		delete_tuple(tuples[ii]);
	}
	delete[] tuples;

	cvReleaseImage(&bkg_temp);
	return background;
};

void PedestrainCounter::create_meanshift_queue(const TCHAR *pIn, const TCHAR* pOut, int r, int pic_num_all)
{
	CvCapture *capture = cvCaptureFromAVI(pIn);
	double fps = cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
	int frameW = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
	int frameH = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
	int framecount = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);

	int frameall_temp = framecount > 3600 ? 3600 : framecount;

	int pic_num = 1; int count = 0;
	int pic_interval = (int)floor(frameall_temp / pic_num_all);
	pic_interval = pic_interval > 35 ? 35 : pic_interval;

	IplImage* img_meanshift = cvCreateImage(cvSize(frameW, frameH), IPL_DEPTH_8U, 1);
	IplImage* frame_gray = cvCreateImage(cvSize(frameW, frameH), IPL_DEPTH_8U, 1);
	IplImage* img_temp = cvCreateImage(cvSize(frameW, frameH), IPL_DEPTH_8U, 1);
	IplImage* img_sum = cvCreateImage(cvSize(frameW, frameH), IPL_DEPTH_32F, 1);
	IplImage* img_zero = cvCreateImage(cvSize(frameW, frameH), IPL_DEPTH_8U, 1);
	IplImage* temp = cvCreateImage(cvSize(frameW, frameH), IPL_DEPTH_8U, 1);
	cvZero(img_zero);
	cvZero(img_sum);


	IplImage* frame = NULL;
	TCHAR infilename[256];
	while (frame = cvQueryFrame(capture))
	{
		if (!(pic_num%pic_interval))
		{
			++count;

			cvCvtColor(frame, frame_gray, CV_RGB2GRAY);
			cvConvert(frame_gray, img_temp);
			cvAcc(img_temp, img_sum);
			cvConvertScale(img_sum, temp, 1.0 / pic_interval);

			cout << "No." << count << ": Current processing frame " << pic_num << endl;

			img_meanshift = pic_meanshift(temp, r);
			sprintf_s(infilename, "%s\\%d.jpg", pOut, count);
			cvSaveImage(infilename, img_meanshift);

			cvZero(img_sum);

			cvReleaseImage(&img_meanshift);
			pic_num++;
		}
		else
		{
			cvCvtColor(frame, frame_gray, CV_RGB2GRAY);
			cvConvert(frame_gray, img_temp);
			cvAcc(img_temp, img_sum);
            pic_num++;
			continue;
		}
		if (pic_num > pic_interval*pic_num_all)
		{
			break;
		}

	}
	cvReleaseImage(&img_meanshift);
	cvReleaseImage(&img_sum);
	cvReleaseImage(&img_temp);
	cvReleaseImage(&temp);
	cvReleaseImage(&img_zero);
	cvReleaseCapture(&capture);

	return;
};

void PedestrainCounter::readin_tuples(TCHAR pIn[], Tuple* &tuples, int pic_num_all, CvRect roi, int frameH, int frameW)
{
	int h = roi.height;
	int w = roi.width;

	TCHAR infilename[100];

	IplImage* img_meanshift = cvCreateImage(cvSize(frameW, frameH), IPL_DEPTH_8U, 1);
	IplImage* img_meanshift_roi = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 1);
	CvMat* Matdata = cvCreateMat(h, w, CV_32FC1);

	for (int i = 0; i < pic_num_all; i++)
	{
		sprintf_s(infilename, "%s\\%d.jpg", pIn, i + 1);
		cout << infilename << endl;

		img_meanshift = cvLoadImage(infilename, 0);
		cvSetImageROI(img_meanshift, roi);
		cvCopy(img_meanshift, img_meanshift_roi);
		cvResetImageROI(img_meanshift);

		cvConvert(img_meanshift_roi, Matdata);
		readin_data(tuples[i], h*w, Matdata);
		cvReleaseImage(&img_meanshift);

	}

	cvReleaseImage(&img_meanshift);
	cvReleaseImage(&img_meanshift_roi);
	cvReleaseMat(&Matdata);

};

void PedestrainCounter::PrintHelp() {
	cout << "Extract background image from video" << endl;
	cout << "HogDetection.exe -b inFile tempFile outFile" << endl;
	cout << "  inFile: The path of video waiting to be detected" << endl;
	cout << "  tempFile: The directory of saving temp pictures during extracting(default = E:\\Human_detection\\result\\temp_meanshift\\)" << endl;
	cout << "  outFile: The path of result to be saved" << endl << endl;

	cout << "Detect Pedestrians in single picture using background cut." << endl;
	cout << "Please FIRST extract the background images from video before detect pedestrians using new method" << endl;
	cout << "HogDetection.exe -p inFile bkg outFile" << endl;
	cout << "  inFile: The path of video waiting to be detected" << endl;
	cout << "  outFile: The path of result to be saved" << endl;
	cout << "  bkg: The path of background picture extracted from video" << endl << endl;

	cout << "Detect Pedestrians in video using new method:" << endl;
	cout << "Please FIRST extract the background images from video before detect pedestrians using new method" << endl;
	cout << "HogDetection.exe -v inFile bkg outFile isdraw isdiff scaleVideo smin smax scalestep slidestep neighbor" << endl;
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

void PedestrainCounter::DetectSinglePictureHOG(const char *in, const char *out,
	const char *adaboost, const Options &opt) {
	// Read in the AdaBoost classifier.
	AdaBoostClassifier classifier(adaboost);

	// read image
	cv::Mat img = cv::imread(in);

	cv::Mat gray(img.size(), CV_8UC1);
	cv::cvtColor(img, gray, cv::COLOR_RGB2GRAY);

	HoGIntegralImage intImage(gray);

	// Dump for testing!!
	intImage.CalculateInt(gray);
	// intImage.Dump("new_integral.txt");

	ImageDetector detector(&classifier, opt);

	// Set the subRegion as the whole image.
	Rect subRegion(0, 0, img.size().width, img.size().height);

	detector.Detect(gray, &intImage, subRegion);
	detector.DrawDetection(img);

	cv::imwrite(out, img);

	// release data buffer
	img.release();
	gray.release();
}

void PedestrainCounter::DetectSinglePictureBKG(const char *in, const char *bkg, const char *out,
	const char *adaboost, const Options &opt) {
	// Read in the AdaBoost classifier.
	AdaBoostClassifier classifier(adaboost);

	// Read image.
	cv::Mat img = cv::imread(in);
	cv::Mat background = cv::imread(bkg, CV_LOAD_IMAGE_GRAYSCALE);

	cv::Mat gray(img.size(), CV_8UC1);
	cv::cvtColor(img, gray, cv::COLOR_RGB2GRAY);

	HoGIntegralImage intImage(gray);
	BKGCutDetector detector(&classifier, opt);

	// Set the subRegion as the whole image.
	Rect subRegion(0, 0, img.size().width, img.size().height);

	// Calculate the integral image.
	intImage.CalculateInt(gray);

	detector.Detect(gray, &intImage, subRegion, background);
	detector.DrawDetection(img);

	cv::imwrite(out, img);

	// release data buffer
	img.release();
	gray.release();

}

void PedestrainCounter::DetectVideoHOG(const char *infile, const char *outfile, const char *adaboost, const Options &opt) {

	AdaBoostClassifier classifier(adaboost);

	// Open the video.
	cv::VideoCapture in(infile);
	if (!in.isOpened()) {
		printf("Can't open video: %s\n", infile);
	}

	// Get the basic inforamtion.
	double fps = in.get(cv::CAP_PROP_FPS);
	int width = (int)in.get(cv::CAP_PROP_FRAME_WIDTH);
	int height = (int)in.get(cv::CAP_PROP_FRAME_HEIGHT);
	int ex = (int)in.get(cv::CAP_PROP_FOURCC);

	// Open the out video.
	cv::VideoWriter out(outfile, ex, fps / 2.0f, cv::Size(width, height));

	// Initialize the detector.
	HoGIntegralImage intImage(width, height);
	ImageDetector imageDetector(&classifier, opt);
	VideoDetector videoDetector(&intImage, &imageDetector, opt);

	// Do the detection.
	videoDetector.Detect(in, out);
}

void PedestrainCounter::DetectVideoBKG(const char *infile, const char *bkgfile, const char *outfile,
	const char *adaboost, const Options &opt) {

	AdaBoostClassifier classifier(adaboost);

	// Read the background.
	cv::Mat bkg = cv::imread(bkgfile, CV_LOAD_IMAGE_GRAYSCALE);

	// Open the video.
	cv::VideoCapture in(infile);
	if (!in.isOpened()) {
		printf("Can't open video: %s\n", infile);
	}

	// Get the basic inforamtion.
	double fps = in.get(cv::CAP_PROP_FPS);
	int width = (int)in.get(cv::CAP_PROP_FRAME_WIDTH);
	int height = (int)in.get(cv::CAP_PROP_FRAME_HEIGHT);
	int ex = (int)in.get(cv::CAP_PROP_FOURCC);

	// Open the out video.
	cv::VideoWriter out(outfile, ex, fps / 2.0f, cv::Size(width, height));

	// Initialize the detector.
	HoGIntegralImage intImage(width, height);
	BKGCutDetector detector(&classifier, opt);
	VideoDetector videoDetector(&intImage, &detector, opt);

	// Do the detection.
	videoDetector.Detect(in, out, bkg);
}

void PedestrainCounter::TrackVideoSingle(const char *infile, const char *outfile) {
	cv::VideoCapture in(infile);
	if (!in.isOpened()) {
		printf("Can't open video: %s\n", infile);
	}

	// Get the basic inforamtion.
	double fps = in.get(cv::CAP_PROP_FPS);
	int width = (int)in.get(cv::CAP_PROP_FRAME_WIDTH);
	int height = (int)in.get(cv::CAP_PROP_FRAME_HEIGHT);
	int ex = (int)in.get(cv::CAP_PROP_FOURCC);

	// Open the out video.
	cv::VideoWriter out(outfile, ex, fps / 2.0f, cv::Size(width, height));

	// Show the first frame and get the roi.
	Rect target;
	cv::Mat first(width, height, CV_8UC3);
	in.read(first);
	cv::imshow("First Frame", first);
	cv::setMouseCallback("First Frame", PedestrainCounter::GetTarget, (void *)&target);
	cv::waitKey(0);
	cv::rectangle(first, (cv::Rect)target, cv::Scalar(255.0f), 2);
	cv::imshow("First Frame", first);
	cv::waitKey(0);
	in.set(cv::CAP_PROP_POS_FRAMES, 0);

#ifdef USE_RGI_FEATURE
	RGIIntegralImage intImage(width, height);
	intImage.CalculateInt(first);
#else
	GrayScaleIntegralImage intImage(width, height);
	//
	// Prepare the integral image.
	//
	cv::Mat grayFirst(width, height, CV_8UC1);
	cv::cvtColor(first, grayFirst, cv::COLOR_RGB2GRAY);
	intImage.CalculateInt(grayFirst);
#endif

	StrongClassifierDirectSelect classifier(50, 250, Size(target.width, target.height), 2);
	ParticleFilter particleFilter(500);
	particleFilter.InitBuffer();
	particleFilter.InitTarget(target);
	SingleSampler sampler(5, 10);

	// Sample.
	sampler.Sample(target, Size(width, height));

	// The first training.
	for (int i = 0; i < sampler.GetNumPos(); i++) {
		classifier.Update(&intImage, sampler.GetPosSample(i), 1);
	}
	for (int i = 0; i < sampler.GetNumNeg(); i++) {
		classifier.Update(&intImage, sampler.GetNegSample(i), -1);
	}

	//
	// Let's see how is the training goes on.
	// Test all the positive samples.
	//
	for (int i = 0; i < sampler.GetNumPos(); i++) {
		float score = classifier.Evaluate(&intImage, sampler.GetPosSample(i));
		printf("Positive Sample %d: %f\n", i, score);
		sampler.DrawSample(first, cv::Scalar(255.0f), i, 1);
		cv::imshow("First Frame", first);
		cv::waitKey(0);
	}

	//
	// Test all the negative samples.
	//
	for (int i = 0; i < sampler.GetNumNeg(); i++) {
		float score = classifier.Evaluate(&intImage, sampler.GetNegSample(i));
		printf("Negative Sample %d: %f\n", i, score);
		sampler.DrawSample(first, cv::Scalar(0.0f, 0.0f, 255.0f), i, -1);
		cv::imshow("First Frame", first);
		cv::waitKey(0);
	}

	ParticleFilterTracker pfTracker(&classifier, &intImage, &particleFilter, &sampler);

	pfTracker.Track(in, out);
}

void PedestrainCounter::TrackVideoMulti(const char *infile, const char *bkgfile, const char *outfile,
	const char *adaboost, const Options &opt) {

	AdaBoostClassifier classifier(adaboost);

	// Read the background.
	cv::Mat bkg = cv::imread(bkgfile, CV_LOAD_IMAGE_GRAYSCALE);

	// Open the video.
	cv::VideoCapture in(infile);
	if (!in.isOpened()) {
		printf("Can't open video: %s\n", infile);
	}

	// Get the basic inforamtion.
	double fps = in.get(cv::CAP_PROP_FPS);
	int width = (int)in.get(cv::CAP_PROP_FRAME_WIDTH);
	int height = (int)in.get(cv::CAP_PROP_FRAME_HEIGHT);
	int ex = (int)in.get(cv::CAP_PROP_FOURCC);

	// Open the out video.
	cv::VideoWriter out(outfile, ex, fps / 2.0f, cv::Size(width, height));

	// Initialize the detector.
	BKGCutDetector detector(&classifier, opt);
	MultiTracker tracker(&detector, Size(width, height), opt);

	// Do the detection.
	tracker.Track(in, out, bkg);
}

void PedestrainCounter::GetTarget(int event, int x, int y, int flags, void *userParam) {
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