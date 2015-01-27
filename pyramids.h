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
class ImageByLevel {
private:
	Mat image;
	int level;
public:
	ImageByLevel(Mat& img, int lev);
	Mat getImage();
	int getLevel();
};

class Pyramids {
private:
	vector<ImageByLevel> imagePyramids;
public:
	Pyramids(Mat& originalImage);
	void computePyramids(ImageByLevel& originalLevel);
	vector<ImageByLevel> getImagePyramids();
	vector<Mat> getPyramids();
};

#endif
