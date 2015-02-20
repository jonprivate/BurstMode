#include <iostream>
#include <stdio.h>
#include <string>
#include "pyramids.h"
#include "imageNode.h"
#include "featuredPyramids.h"
#include <opencv2/opencv.hpp>
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/nonfree.hpp"
using namespace cv;
//using namespace std;

int main()
{
    Mat originalImage1 = imread( "../../../data/burstimages_v1/Baby/5.jpg" );
    Pyramids pyramids1(originalImage1);
    vector<Mat> imagePyramids1 = pyramids1.getPyramids();
    Mat topImage1 = imagePyramids1[0];
    Mat originalImage2 = imread( "../../../data/burstimages_v1/Baby/6.jpg" );
    Pyramids pyramids2(originalImage2);
    vector<Mat> imagePyramids2 = pyramids2.getPyramids();
    Mat topImage2 = imagePyramids2[0];
    //Mat topImage2 = imagePyramids1[0];
    //testImageNode();
    testFeaturedImagePyramids();
    
    //-- create detector and descriptor --
    SurfFeatureDetector detector(400);
    //GoodFeaturesToTrackDetector detector(200, 0.01, 1.);
    SurfDescriptorExtractor descriptor;
    //BriefDescriptorExtractor descriptor(16);

    // detect keypoints
    std::vector<KeyPoint> keypoints1, keypoints2;
    detector.detect(topImage1, keypoints1);
    detector.detect(topImage2, keypoints2);
    std::cout << keypoints1[0].size << std::endl;

    std::cout << keypoints1.size() << std::endl;
    std::cout << keypoints2.size() << std::endl;
    int num1 = keypoints1.size(), num2 = keypoints2.size();

    // extract features
    cv::Mat desc1, desc2;
    descriptor.compute(topImage1, keypoints1, desc1);
    descriptor.compute(topImage2, keypoints2, desc2);
    /*desc1.convertTo(desc1, CV_32F);
    desc1 = desc1 / 255;
    desc2.convertTo(desc2, CV_32F);
    desc2 = desc2 / 255;*/

    // ... your code for matching and creating homography-matrix
    Mat img1 = topImage1.clone();
    Mat img2 = topImage2.clone();
    for (int i = 0; i < keypoints1.size(); i++) {
        circle(img1, keypoints1[i].pt, 2, Scalar(0,255,0));
    }
    for (int i = 0; i < keypoints2.size(); i++) {
        circle(img2, keypoints2[i].pt, 2, Scalar(0,255,0));
    }
    std::string window_name1 ("feature points 1");
    namedWindow(window_name1);
    imshow(window_name1, img1);
    std::string window_name2 ("feature points 2");
    namedWindow(window_name2);
    imshow(window_name2, img2);
    waitKey(0);

    //-- Step 3: Matching descriptor vectors using FLANN matcher
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

    Mat img_matches;
    drawMatches( img1, keypoints1, img2, keypoints2,
               good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
               vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

    //-- Localize the object
    std::vector<Point2f> feat1;
    std::vector<Point2f> feat2;

    for( int i = 0; i < good_matches.size(); i++ )
    {
        //-- Get the keypoints from the good matches
        feat1.push_back( keypoints1[ good_matches[i].queryIdx ].pt );
        feat2.push_back( keypoints2[ good_matches[i].trainIdx ].pt );
    }

    Mat H = findHomography( feat1, feat2, CV_RANSAC );
    std::cout << H << std::endl;
    
    //-- Get the corners from the image_1 ( the object to be "detected" )
    std::vector<Point2f> obj_corners(4);
    obj_corners[0] = cvPoint(0,0); obj_corners[1] = cvPoint( img1.cols, 0 );
    obj_corners[2] = cvPoint( img1.cols, img1.rows ); obj_corners[3] = cvPoint( 0, img1.rows );
    std::vector<Point2f> scene_corners(4);

    perspectiveTransform( obj_corners, scene_corners, H);

    //-- Draw lines between the corners (the mapped object in the scene - image_2 )
    line( img_matches, scene_corners[0] + Point2f( img1.cols, 0), scene_corners[1] + Point2f( img1.cols, 0), Scalar(0, 255, 0), 4 );
    line( img_matches, scene_corners[1] + Point2f( img1.cols, 0), scene_corners[2] + Point2f( img1.cols, 0), Scalar( 0, 255, 0), 4 );
    line( img_matches, scene_corners[2] + Point2f( img1.cols, 0), scene_corners[3] + Point2f( img1.cols, 0), Scalar( 0, 255, 0), 4 );
    line( img_matches, scene_corners[3] + Point2f( img1.cols, 0), scene_corners[0] + Point2f( img1.cols, 0), Scalar( 0, 255, 0), 4 );

    //-- Show detected matches
    imshow( "Good Matches & Object detection", img_matches );

    Mat outimg = img1.clone();
    warpPerspective(img1, outimg, H, outimg.size());
    namedWindow("warped", CV_WINDOW_AUTOSIZE );
    imshow("warped", outimg );

    waitKey(0);

    std::cout << keypoints1[0].pt << std::endl;
    keypoints1[0].pt *= 2;
    std::cout << keypoints1[0].pt << std::endl;

    return 0;
}
