# Pedestrain Tracker
### Intro
This is the group project of Digital Image Processing, Tsinghua University, 2015 Spring. We have three great partners:

- [Zhengrong Wang](https://zerowong.github.io/)
- [Hsienyu Meng](https://github.com/xianyuMeng)
- [Liuyang Zhan](https://github.com/zhanlythu)

We tried to do some simple pedestrain tracking based on detection. Use it at your own risk.

### Usage
The only third party library you will need is [OpenCV 3.0.0](http://opencv.org/downloads.html). And please compile it with VS 2013. Other platform should be fine, but I haven't tested it.

* `-b srcVideo tempDiectorary outputImage` : Use mean-shift to construct the background.
* `-phog srcImage outputImage` : Apply HOG detector to a single image.
* `-p srcImage bkgImage outputImage` : Apply BKG Subtraction + HOG to a single image.
* `-vhog srcVideo outputVideo` : Apply HOG detector to a video.
* `-v srcVideo bkgImage outputVideo` : Apply BKG Subtraction + HOG to a video.
* `--particle-tracker srcVideo outputVideo` : Apply particle filter to track a single target in the video.
* `--multiple-tracker srcVideo bkgImage outputVideo` : Use detector and particle filter to tracker multiple targets.
### Detection
For detection part we use simple HOG feature and AdaBoost classifier. We rewrite the baseline program in `C++` style and improve the perfomance.
### Tracking
We use particle filter with a online-boosting classifier with RGI feature for each target/pedestrain. We use match matrix and greedy algorithm to handle the data association problem. The main idea is based on this [paper](http://www.mmp.rwth-aachen.de/publications/pdf/breitenstein-detectorconfidencefilter-iccv09.pdf).
### Result
Here is a simple [project website](https://zerowong.github.io/PedestrainTracker). You can find everything there.

* [Documents](https://zerowong.github.io/PedestrainTracker/docs/html/index.html)
* [Final Report](https://zerowong.github.io/PedestrainTracker/report/FinalReport.pdf)
* [Mean-shift Background Image](https://zerowong.github.io/PedestrainTracker/results/Background/S3MF1.jpg)
* [Detector for a video](https://zerowong.github.io/PedestrainTracker/results/VideoDetector/BKG_reconstructed_1.avi)
* [Single target tracking result with Gray Haar feature](https://zerowong.github.io/PedestrainTracker/results/ParticleFilterSingleTracker/singleTrackerTestGray01.avi)
* [Single target tracking result with RGI feature](https://zerowong.github.io/PedestrainTracker/results/ParticleFilterSingleTracker/singleTrackerTestRGI01.avi)
* [Yet another single target tracking result](https://zerowong.github.io/PedestrainTracker/results/ParticleFilterSingleTracker/S3MF1_single_tracker.avi)
* [Pedestrain Counter result](https://zerowong.github.io/PedestrainTracker/results/ParticleFilterSingleTracker/S3MF1_multiple_tracker.avi)
