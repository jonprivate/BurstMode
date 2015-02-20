#include "imageNode.h"
#include <cmath>
#include "opencv2/nonfree/nonfree.hpp"
#include "pyramids.h"
#include <iostream>

// class ImageNode
ImageNode::ImageNode(Mat& p, Rect r) {
	image = p;
	rect = r;
}

ImageNode::ImageNode(const ImageNode& imgNode) {
    image = imgNode.getImage();
    rect = imgNode.getRect();
}

Mat ImageNode::getImage() const{
    return image;
}

Mat ImageNode::getPatch() const{
	Mat roi(image, rect);
    return roi;
}

Rect ImageNode::getRect() const{
	return rect;
}

// class FeaturedImageNode, inherited from ImageNode
FeaturedImageNode::FeaturedImageNode(Mat& img, Rect r, vector<KeyPoint> keypts, int level): ImageNode(img, r) {
	if (level == 0) {
		keypoints = keypts;
	} else {
        int mult = pow(2, level);
        Rect baseRect(r.x / mult, r.y / mult, r.width / mult, r.height / mult);
#ifdef TEST
    printf("initial rect\n");
    std::cout << r << std::endl;
    printf("base rect\n");
    std::cout << baseRect << std::endl;
#endif
		for (int i = 0; i < keypts.size(); i++) {
            if (baseRect.contains(keypts[i].pt)) {
                KeyPoint tmp(keypts[i].pt.x * mult, keypts[i].pt.y * mult, keypts[i].size, keypts[i].angle,
                keypts[i].response, keypts[i].octave, keypts[i].class_id);
                keypoints.push_back(tmp);
            }
		}
	}
#ifdef TEST
    printf("Number of Key Points in the constructor: %d\n", keypoints.size());
#endif
}

FeaturedImageNode::FeaturedImageNode(ImageNode imgNode, vector<KeyPoint> keypts, int level): ImageNode(imgNode) {
    if (level == 0) {
        keypoints = keypts;
    } else {
        int mult = pow(2, level);
        Rect r = imgNode.getRect();
        Rect baseRect(r.x / mult, r.y / mult, r.width / mult, r.height / mult);
        for (int i = 0; i < keypts.size(); i++) {
            if (baseRect.contains(keypts[i].pt)) {
                KeyPoint tmp(keypts[i].pt.x * mult, keypts[i].pt.y * mult, keypts[i].size, keypts[i].angle,
                keypts[i].response, keypts[i].octave, keypts[i].class_id);
                keypoints.push_back(tmp);
            }
        }
    }
}

void FeaturedImageNode::computeDescriptors(Mat& desc) {
	SurfDescriptorExtractor descriptor;
    //BriefDescriptorExtractor descriptor(16);
	descriptor.compute(image, keypoints, desc);
    /*desc1.convertTo(desc1, CV_32F);
    desc1 = desc1 / 255;
    desc2.convertTo(desc2, CV_32F);
    desc2 = desc2 / 255;*/
}

void FeaturedImageNode::computeHomography(FeaturedImageNode dst, Mat& H) {
    // ... your code for matching and creating homography-matrix
    /*************************************************************************/
#ifdef TEST
    Mat i1 = image.clone();
    Mat i2 = dst.getImage().clone();
    vector<KeyPoint> kpts1 = this->getKeyPoints();
    vector<KeyPoint> kpts2 = dst.getKeyPoints();
    for (int i = 0; i < kpts1.size(); i++) {
        circle(i1, kpts1[i].pt, 2, Scalar(0,255,0));
    }
    for (int i = 0; i < kpts2.size(); i++) {
        circle(i2, kpts2[i].pt, 2, Scalar(0,255,0));
    }
    std::string window_name1 ("feature points 1");
    namedWindow(window_name1);
    imshow(window_name1, i1);
    std::string window_name2 ("feature points 2");
    namedWindow(window_name2);
    imshow(window_name2, i2);
    waitKey(0);
#endif
    /*************************************************************************/
	// compute descriptors using keypoints
	vector<KeyPoint> keypoints1 = this->getKeyPoints();
	vector<KeyPoint> keypoints2 = dst.getKeyPoints();
    cout << keypoints1.size() << endl;
    cout << keypoints2.size() << endl;
	cv::Mat desc1, desc2;
	this->computeDescriptors(desc1);
	dst.computeDescriptors(desc2);
#ifdef TEST
    printf("Show keypoints and corresponding image size:\n");
    for (int i = 0; i < keypoints1.size(); i++) {
        printf("keypoints[%d]:%f,%f\n", i, keypoints1[i].pt.x, keypoints1[i].pt.y);
    }
    printf("Image size: %d x %d\n", this->getImage().rows, this->getImage().cols);
#endif
	// match with descriptors
	FlannBasedMatcher matcher;
    std::vector< DMatch > matches;
    matcher.match( desc1, desc2, matches );
    double max_dist = 0; double min_dist = 100;
    //-- Quick calculation of max and min distances between keypoints
    for( int i = 0; i < desc1.rows; i++ )
    {
        double dist = matches[i].distance;
        if( dist < min_dist ) min_dist = dist;
        if( dist > max_dist ) max_dist = dist;
    }
    //-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
    std::vector< DMatch > good_matches;
    for( int i = 0; i < desc1.rows; i++ )
    {
        if( matches[i].distance < min_dist * 3 )
            good_matches.push_back( matches[i]);
    }

    /*************************************************************************/
#ifdef TEST
    Mat img_matches;
    drawMatches( i1, keypoints1, i2, keypoints2,
               good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
               vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
    imshow( "Good Matches & Object detection", img_matches );
    printf("Number of good matches: %d\n", good_matches.size());
#endif
    /*************************************************************************/

	// compute homography
    std::vector<Point2f> feat1;
    std::vector<Point2f> feat2;

    for( int i = 0; i < good_matches.size(); i++ )
    {
        //-- Get the keypoints from the good matches
        feat1.push_back( keypoints1[ good_matches[i].queryIdx ].pt );
        feat2.push_back( keypoints2[ good_matches[i].trainIdx ].pt );
    }

    H = findHomography( feat1, feat2, CV_RANSAC );
}

int FeaturedImageNode::numOfKeyPoints() {
	return keypoints.size();
}

vector<KeyPoint> FeaturedImageNode::getKeyPoints() {
	return keypoints;
}

int testImageNode() {
    Mat originalImage1 = imread( "../../../data/burstimages_v1/Baby/5.jpg" );
    Pyramids pyramids1(originalImage1);
    vector<Mat> imagePyramids1 = pyramids1.getPyramids();
    Mat topImage1 = imagePyramids1[0];
    Mat originalImage2 = imread( "../../../data/burstimages_v1/Baby/6.jpg" );
    Pyramids pyramids2(originalImage2);
    vector<Mat> imagePyramids2 = pyramids2.getPyramids();
    Mat topImage2 = imagePyramids2[0];

    // x, y, width, height
    Rect r = Rect(180 , 320, 180, 320);
    ImageNode imgNode1(originalImage1, r);
    ImageNode imgNode2(originalImage2, r);
    std::cout << "successfully constructed" << std::endl;
    // detect keypoints
    SurfFeatureDetector detector(400);
    std::vector<KeyPoint> keypoints1, keypoints2;
    detector.detect(topImage1, keypoints1);
    detector.detect(topImage2, keypoints2);
    // create FeaturedImageNode
    FeaturedImageNode featuredImgNode1(imgNode1, keypoints1, 2);
    FeaturedImageNode featuredImgNode2(imgNode2, keypoints2, 2);
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