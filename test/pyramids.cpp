#include "./pyramids.h"

// Basic class: ImageByLevel
ImageByLevel::ImageByLevel(Mat& img, int lev)
{
	image = img;
	level = lev;
}

Mat ImageByLevel::getImage() const
{
	return image;
}
int ImageByLevel::getLevel() const
{
	return level;
}

// Higher level class: Pyramids
Pyramids::Pyramids(Mat& originalImage)
{
    int rownum = originalImage.rows, colnum = originalImage.cols;
    int totalLevel = 1;
    while(rownum >= MIN_PIXELS_ON_EACH_DIM || colnum >= MIN_PIXELS_ON_EACH_DIM) {
    	rownum /= 2;
    	colnum /= 2;
    	totalLevel++;
    }
    ImageByLevel originalLevel(originalImage, totalLevel - 1);
    imagePyramids.push_back(originalLevel);
    computePyramids(originalLevel);
}

void Pyramids::computePyramids(ImageByLevel& originalLevel)
{
    Mat tmp = originalLevel.getImage(), dst;
#ifdef TEST
    printf("Size of original level: %drows, %dcols\n", tmp.rows, tmp.cols);
#endif
    int topLevel = originalLevel.getLevel();
    int currentLevel = topLevel - 1;
    while(tmp.rows >= MIN_PIXELS_ON_EACH_DIM || tmp.cols >= MIN_PIXELS_ON_EACH_DIM) {
    	pyrDown( tmp, dst, Size( tmp.cols/2, tmp.rows/2 ) );
    	ImageByLevel currentLevelImage(dst, currentLevel);
        imagePyramids.insert(imagePyramids.begin(), currentLevelImage);
#ifdef TEST
        printf("Size of current level: %drows, %dcols\n", dst.rows, dst.cols);
#endif
        tmp = dst;
    }
}

vector<ImageByLevel> Pyramids::getImagePyramids()
{
	return imagePyramids;
}

vector<Mat> Pyramids::getPyramids()
{
	vector<ImageByLevel> imgPyramids = getImagePyramids();
	vector<Mat> pyramids;
	for (int i = 0; i < imgPyramids.size(); i++) {
		pyramids.push_back(imgPyramids[i].getImage());
	}
	return pyramids;
}
