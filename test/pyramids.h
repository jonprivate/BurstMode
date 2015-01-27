#ifndef _PYRAMIDS_H_
#define _PYRAMIDS_H_

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <vector>

using namespace cv;

static const int MIN_PIXELS_ON_EACH_DIM = 400;

class Pyramids {
private:
	vector<Mat> imagePyramids;
public:
	Pyramids(Mat& originalImage);
	void computePyramids(Mat& originalImage);
	vector<Mat> getPyramids();
};

#endif
