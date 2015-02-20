#ifndef _IMAGENODE_H_
#define _IMAGENODE_H_

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include "mode.h"

using namespace cv;
using namespace std;

class ImageNode {
protected:
	Mat image;
	Rect rect;
public:
	ImageNode(Mat& img, Rect r);
	ImageNode(const ImageNode& imgNode);
	Mat getImage() const;
	Mat getPatch() const;
	Rect getRect() const;
};

class FeaturedImageNode : public ImageNode {
private:
	vector<KeyPoint> keypoints;
public:
	// initialize with the corresponding patch or the coarse image (determined according to isImage)
	FeaturedImageNode(Mat& img, Rect r, vector<KeyPoint> keypts, int level = 0);
	FeaturedImageNode(ImageNode imgNode, vector<KeyPoint> keypts, int level = 0);
	void computeDescriptors(Mat& desc);
	void computeHomography(FeaturedImageNode dst, Mat& H);
	int numOfKeyPoints();
	vector<KeyPoint> getKeyPoints();
};

int testImageNode();

#endif