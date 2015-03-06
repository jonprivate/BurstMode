#ifndef _HOMOGRAPHYPYRAMIDS_H_
#define _HOMOGRAPHYPYRAMIDS_H_

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include "imageNode.h"
#include "pyramids.h"
#include "featuredPyramids.h"

inline int indexOfRowColumn(int row, int col, int colsPerRow) {
	return (row - 1) * colsPerRow + col;
}

inline void rowColumnOfIndex(int i, int colsPerRow, int& row, int& col) {
	row = i / colsPerRow;
	col = i % colsPerRow;
}

class NodeHomography {
protected:
	Mat H;
public:
	NodeHomography(Mat& h): H(h) {}
	NodeHomography(FeaturedImageNode srcNode, FeaturedImageNode dstNode);
	Mat computeNodeHomography(FeaturedImageNode srcNode, FeaturedImageNode dstNode);
	Mat getNodeHomography();
};

class PairedImageHomography {
protected:
	vector<NodeHomography> nodeHomographies;
	int colsPerRow;
public:
	PairedImageHomography(vector<NodeHomography> Hs, int cols);
	PairedImageHomography(FeaturedImage srcImage, FeaturedImage dstImage);
	vector<Mat> computeImageHomography(FeaturedImage srcImage, FeaturedImage dstImage);
	vector<Mat> getImageHomography();
	Mat getNodeHomographyByIndex(int i);
	vector<Mat> getNeighborHomographies(int i);
	int getColsPerRow();
};

class PairedPyramidHomography {
protected:
	vector<PairedImageHomography> pairedImageHomographies;
public:
	PairedPyramidHomography(vector<PairedImageHomography> IHs);
	PairedPyramidHomography(FeaturedImagePyramids srcPyramid, FeaturedImagePyramids dstPyramid);
	vector<vector<Mat>> computePyramidHomography(FeaturedImagePyramids srcPyramid, FeaturedImagePyramids dstPyramid);
	vector<vector<Mat>> getPyramidHomography();
	vector<Mat> getImageHomographyByLevel(int level);
	Mat getNodeHomographyByLevelAndIndex(int level, int i);
	vector<Mat> getNeighborHomographies(int level, int i);
	Mat getUpperLevelHomography(int level, int i);
	vector<vector<Mat>> refineHomographies();
	Mat discretizeHomography(int rows, int cols, Mat& xflow, Mat& yflow);
};

class PairedFeaturedImagePyramids {
protected:
	FeaturedImagePyramids reference;
	FeaturedImagePyramids companion;
	PairedPyramidHomography pairedHomographies;
	vector<Mat> homographyFlow;
public:
	PairedFeaturedImagePyramids(FeaturedImagePyramids ref, FeaturedImagePyramids comp, PairedPyramidHomography PHs);
	PairedFeaturedImagePyramids(FeaturedImagePyramids ref, FeaturedImagePyramids comp);
	vector<vector<Mat>> computePyramidHomography();
	vector<vector<Mat>> getPyramidHomography();
	vector<Mat> getImageHomographyByLevel(int level);
	Mat getNodeHomographyByLevelAndIndex(int level, int i);
};

#endif