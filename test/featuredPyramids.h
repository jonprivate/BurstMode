#ifndef _FEATUREDPYRAMIDS_H_
#define _FEATUREDPYRAMIDS_H_

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include "imageNode.h"
#include "pyramids.h"
#include "mode.h"

using namespace std;
using namespace cv;

class FeaturedImage : public ImageByLevel {
protected:
	vector<FeaturedImageNode> nodes;
public:
	// constructor
	FeaturedImage(vector<FeaturedImageNode> nds, Mat& img, int lev);
	FeaturedImage(vector<KeyPoint> kpts, Mat& img, int lev);
	int getNodeNumber();
	vector<FeaturedImageNode> getFeaturedImageNodes();
	FeaturedImageNode getFeaturedImageNodeByIndex(int i);
};

class FeaturedImagePyramids {
private:
	vector<FeaturedImage> featuredImages;
	vector<KeyPoint> totalKeyPoints;
public:
	// constructor
	FeaturedImagePyramids(Mat& originalImage);
	void computePyramids(FeaturedImage& originalLevel);
	vector<KeyPoint> detectFeaturePoints(Mat& baseImage);

	vector<FeaturedImage> getImagePyramids();
	FeaturedImage getFeaturedImageByLevel(int level);
	vector<Mat> getPyramids();
	vector<KeyPoint> getKeyPoints();
	int getLevelNumber();
	int getNodeNumberByLevel(int level);
};

int testFeaturedImagePyramids();

#endif