#include "featuredPyramids.h"
#include <iostream>
#include <cmath>
#include "opencv2/nonfree/nonfree.hpp"

using namespace std;
using namespace cv;

FeaturedImage::FeaturedImage(vector<FeaturedImageNode> nds, Mat& img, int lev): ImageByLevel(img, lev) {
	nodes = nds;
}

FeaturedImage::FeaturedImage(vector<KeyPoint> kpts, Mat& img, int lev): ImageByLevel(img, lev) {
	int colsPerRow = pow(2, lev);
	int rowsPerCol = colsPerRow;
	int pixPerCol = img.cols / colsPerRow;
	int pixPerRow = img.rows / rowsPerCol;
#ifdef TEST
	std::cout << "Number of Nodes: " << rowsPerCol * colsPerRow << std::endl;
	printf("Number of KeyPoints: %d\n", kpts.size());
	int i = 0;
#endif
	for (int row = 0; row < rowsPerCol; row++) {
		int rowStart = row * pixPerRow;
		//int rowEnd = (row + 1) * pixPerRow - 1;
		for (int col = 0; col < colsPerRow; col++) {
			int colStart = col * pixPerCol;
			//int colEnd = (col + 1) * pixPerCol;
			Rect rect(colStart, rowStart, pixPerRow, pixPerCol);

			FeaturedImageNode node(img, rect, kpts, lev);
			nodes.push_back(node);
#ifdef TEST
	printf("Number of KeyPoints in node %d: %d\n", i++, node.numOfKeyPoints());
#endif
		}
	}
}

int FeaturedImage::getNodeNumber() {
	return nodes.size();
}

vector<FeaturedImageNode> FeaturedImage::getFeaturedImageNodes() {
	return nodes;
}

FeaturedImageNode FeaturedImage::getFeaturedImageNodeByIndex(int i) {
	return nodes[i];
}

/*****************************************************************/
FeaturedImagePyramids::FeaturedImagePyramids(Mat& originalImage) {
	int rownum = originalImage.rows, colnum = originalImage.cols;
    int totalLevel = 1;
    while(rownum >= MIN_PIXELS_ON_EACH_DIM || colnum >= MIN_PIXELS_ON_EACH_DIM) {
    	rownum /= 2;
    	colnum /= 2;
    	totalLevel++;
    }
    int topLevel = totalLevel - 1;
    /*
    int baseRows = originalImage.rows / pow(2, topLevel);
    int baseCols = originalImage.cols / pow(2, topLevel);
    Mat base;
    pyrDown( originalImage, base, Size( baseCols, baseRows ) );*/
    vector<Mat> images;
    images.push_back(originalImage);
    Mat tmp = originalImage, dst;
    while(tmp.rows >= MIN_PIXELS_ON_EACH_DIM || tmp.cols >= MIN_PIXELS_ON_EACH_DIM) {
    	pyrDown( tmp, dst, Size( tmp.cols/2, tmp.rows/2 ) );
    	images.insert(images.begin(), dst);
        tmp = dst;
    }
    Mat base = images[0];
    detectFeaturePoints(base);
#ifdef TEST
    printf("Number of totalKeyPoints: %d\n", totalKeyPoints.size());
#endif
    for (int i = 0; i < images.size(); i++) {
    	FeaturedImage currentLevelImage(totalKeyPoints, images[i], i);
        featuredImages.push_back(currentLevelImage);
        tmp = dst;
    }
}

void FeaturedImagePyramids::computePyramids(FeaturedImage& originalLevel) {
    Mat tmp = originalLevel.getImage(), dst;
    int topLevel = originalLevel.getLevel();
    int currentLevel = topLevel - 1;
    while(tmp.rows >= MIN_PIXELS_ON_EACH_DIM || tmp.cols >= MIN_PIXELS_ON_EACH_DIM) {
    	pyrDown( tmp, dst, Size( tmp.cols/2, tmp.rows/2 ) );
    	FeaturedImage currentLevelImage(totalKeyPoints, dst, currentLevel);
        featuredImages.insert(featuredImages.begin(), currentLevelImage);
        tmp = dst;
    }
}

vector<KeyPoint> FeaturedImagePyramids::detectFeaturePoints(Mat& baseImage) {
	// detect keypoints
    SurfFeatureDetector detector(400);
    detector.detect(baseImage, totalKeyPoints);
    return totalKeyPoints;
}

vector<FeaturedImage> FeaturedImagePyramids::getImagePyramids() {
	return featuredImages;
}

FeaturedImage FeaturedImagePyramids::getFeaturedImageByLevel(int level) {
	return featuredImages[level];
}

vector<Mat> FeaturedImagePyramids::getPyramids() {
	vector<FeaturedImage> imgPyramids = getImagePyramids();
	vector<Mat> pyramids;
	for (int i = 0; i < imgPyramids.size(); i++) {
		pyramids.push_back(imgPyramids[i].getImage());
	}
	return pyramids;
}

vector<KeyPoint> FeaturedImagePyramids::getKeyPoints() {
	return totalKeyPoints;
}

int FeaturedImagePyramids::getLevelNumber() {
	return featuredImages.size();
}

int FeaturedImagePyramids::getNodeNumberByLevel(int level) {
	return featuredImages[level].getNodeNumber();
}

int testFeaturedImagePyramids() {
	Mat originalImage1 = imread( "../../../data/burstimages_v1/Baby/5.jpg" );
    FeaturedImagePyramids featuredImagePyramids1(originalImage1);
    Mat originalImage2 = imread( "../../../data/burstimages_v1/Baby/6.jpg" );
    FeaturedImagePyramids featuredImagePyramids2(originalImage2);

    int level = featuredImagePyramids1.getLevelNumber() - 1;
    int nodeInd = 0;
    FeaturedImage featuredImage1 = featuredImagePyramids1.getFeaturedImageByLevel(level);
    std::cout << featuredImage1.getNodeNumber() << std::endl;
    FeaturedImageNode featuredImgNode1 = featuredImage1.getFeaturedImageNodeByIndex(nodeInd);
#ifdef TEST
    printf("Number of keypoints in featuredImgNode: %d\n", featuredImgNode1.getKeyPoints().size());
#endif
    FeaturedImage featuredImage2 = featuredImagePyramids2.getFeaturedImageByLevel(level);
    std::cout << featuredImage2.getNodeNumber() << std::endl;
    FeaturedImageNode featuredImgNode2 = featuredImage2.getFeaturedImageNodeByIndex(nodeInd);

    // compute homography
    Mat H;
    featuredImgNode1.computeHomography(featuredImgNode2, H);
    std::cout << H << std::endl;
    std::cout << "SUCCESS" << std::endl;

    Mat outimg = originalImage1.clone();
    warpPerspective(originalImage1, outimg, H, outimg.size());
    namedWindow("warped", CV_WINDOW_AUTOSIZE );
    imshow("warped", outimg );

    waitKey(0);

    return 0;
}