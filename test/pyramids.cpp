#include "./pyramids.h"

Pyramids::Pyramids(Mat& originalImage)
{
    imagePyramids.push_back(originalImage);
    computePyramids(originalImage);
}

void Pyramids::computePyramids(Mat& originalImage)
{
    Mat tmp = originalImage, dst;
    while(tmp.rows >= MIN_PIXELS_ON_EACH_DIM || tmp.cols >= MIN_PIXELS_ON_EACH_DIM) {
            pyrDown( tmp, dst, Size( tmp.cols/2, tmp.rows/2 ) );
            imagePyramids.push_back(dst);
            tmp = dst;
    }
}

vector<Mat> Pyramids::getPyramids()
{
	return imagePyramids;
}
