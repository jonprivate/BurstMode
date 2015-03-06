#include "homographyPyramids.h"
NodeHomography::NodeHomography(FeaturedImageNode srcNode, FeaturedImageNode dstNode) {
	H = computeNodeHomography(srcNode, dstNode);
}

Mat NodeHomography::computeNodeHomography(FeaturedImageNode srcNode, FeaturedImageNode dstNode) {
	Mat homography;
	srcNode.computeHomography(dstNode, homography);
	return homography;
}

Mat NodeHomography::getNodeHomography() {
	return H;
}

/*********************************************************************************************/
PairedImageHomography::PairedImageHomography(vector<NodeHomography> Hs, int cols) {
	nodeHomographies = Hs;
	colsPerRow = cols;
}

PairedImageHomography::PairedImageHomography(FeaturedImage srcImage, FeaturedImage dstImage) {
	computeImageHomography(srcImage, dstImage);
	colsPerRow = srcImage.getFeaturedImageNodeByIndex(0).getRect().width;
}

vector<Mat> PairedImageHomography::computeImageHomography(FeaturedImage srcImage, FeaturedImage dstImage) {
	int nodeNum = srcImage.getNodeNumber();
	vector<Mat> homographies;
	for (int i = 0; i < nodeNum; i++) {
		FeaturedImageNode srcNode = srcImage.getFeaturedImageNodeByIndex(i);
		FeaturedImageNode dstNode = dstImage.getFeaturedImageNodeByIndex(i);
		NodeHomography nodeHomography(srcNode, dstNode);
		nodeHomographies.push_back(nodeHomography);
		homographies.push_back(nodeHomography.getNodeHomography());
	}
	return homographies;
}

vector<Mat> PairedImageHomography::getImageHomography() {
	vector<Mat> homographies;
	for (int i = 0; i < nodeHomographies.size(); i++) {
		homographies.push_back(nodeHomographies[i].getNodeHomography());
	}
	return homographies;
}

Mat PairedImageHomography::getNodeHomographyByIndex(int i) {
	return nodeHomographies[i].getNodeHomography();
}

vector<Mat> PairedImageHomography::getNeighborHomographies(int i) {
	int row, col;
	rowColumnOfIndex(i, colsPerRow, row, col);
	vector<Mat> homographies;
	int ind;
	ind = indexOfRowColumn(row - 1, col, colsPerRow);
	if (ind >= 0 && ind < nodeHomographies.size()) {
		homographies.push_back(nodeHomographies[ind].getNodeHomography());
	}
	ind = indexOfRowColumn(row + 1, col, colsPerRow);
	if (ind >= 0 && ind < nodeHomographies.size()) {
		homographies.push_back(nodeHomographies[ind].getNodeHomography());
	}
	ind = indexOfRowColumn(row, col - 1, colsPerRow);
	if (ind >= 0 && ind < nodeHomographies.size()) {
		homographies.push_back(nodeHomographies[ind].getNodeHomography());
	}
	ind = indexOfRowColumn(row, col + 1, colsPerRow);
	if (ind >= 0 && ind < nodeHomographies.size()) {
		homographies.push_back(nodeHomographies[ind].getNodeHomography());
	}
}

int PairedImageHomography::getColsPerRow() {
	return colsPerRow;
}


/*********************************************************************************************/
PairedPyramidHomography::PairedPyramidHomography(vector<PairedImageHomography> IHs) {
	PairedImageHomographies = IHs;
}

PairedPyramidHomography::PairedPyramidHomography(FeaturedImagePyramids srcPyramid, FeaturedImagePyramids dstPyramid) {
	int levelNum = srcPyramid.getLevelNumber();
	for (int l = 0; l < levelNum; l++) {
		FeaturedImage srcImage = srcPyramid.getFeaturedImageByLevel(l);
		FeaturedImage dstImage = dstPyramid.getFeaturedImageByLevel(l);
		PairedImageHomography pairedHomography(srcImage, dstImage);
		pairedImageHomographies.push_back(pairedHomography);
	}
}

vector<vector<Mat>> PairedPyramidHomography::computePyramidHomography(FeaturedImagePyramids srcPyramid, FeaturedImagePyramids dstPyramid) {
	int levelNum = srcPyramid.getLevelNumber();
	vector<vector<Mat>> pihs;
	for (int l = 0; l < levelNum; l++) {
		FeaturedImage srcImage = srcPyramid.getFeaturedImageByLevel(l);
		FeaturedImage dstImage = dstPyramid.getFeaturedImageByLevel(l);
		PairedImageHomography pairedHomography(srcImage, dstImage);
		pairedImageHomographies.push_back(pairedHomography);
		pihs.push_back(pairedHomography.getImageHomography());
	}
	return pihs;
}

vector<vector<Mat>> PairedPyramidHomography::getPyramidHomography() {
	vector<vector<Mat>> pihs;
	for (int i = 0; i < pairedImageHomographies.size(); i++) {
		pihs.push_back(pairedImageHomographies[i].getImageHomography());
	}
	return pihs;
}

vector<Mat> PairedPyramidHomography::getImageHomographyByLevel(int level) {
	return pairedImageHomographies[level].getImageHomography();
}

Mat PairedPyramidHomography::getNodeHomographyByLevelAndIndex(int level, int i) {
	vector<NodeHomography> pihs = pairedImageHomographies[level].getImageHomography();
	return pihs[i];
}

vector<Mat> PairedPyramidHomography::getNeighborHomographies(int level, int i) {
	return pairedImageHomographies[level].getNeighborHomographies(i);
}

Mat PairedPyramidHomography::getUpperLevelHomography(int level, int i) {
	int row, col;
	int colsPerRow = pairedImageHomographies[level].getColsPerRow();
	rowColumnOfIndex(i, colsPerRow, row, col);
	return pairedImageHomographies[level].getNodeHomographyByIndex(indexOfRowColumn(row / 2, col / 2, colsPerRow));
}

vector<vector<Mat>> PairedPyramidHomography::refineHomographies() {
	// this is a fake version, just to complete this part for test
}

void PairedPyramidHomography::discretizeHomography(int rows, int cols, Mat& xflow, Mat& yflow) {
	// get the finest level homographies
	int level = pairedImageHomographies.size() - 1;
	vector<Mat> homographies = getImageHomographyByLevel(level);
	// get alignments of image nodes and pixel # in each node
	int colsPerRow = pairedImageHomographies[0].getColsPerRow();
	int pixelsPerCellCol = rows / colsPerRow;
	int pixelsPerCellRow = cols / colsPerRow;
	// go through all nodes and compute flow for each node, store those in xflow and yflow
	for (int i = 0; i < homographies.size(); i++) {
		// first compute the position of the current image node
		int row, col;
		rowColumnOfIndex(i, colsPerRow, row, col);
		// then map onto the real image pixel axis
		int start_row = row * pixelsPerCellCol, end_row = (row + 1) * pixelsPerCellCol - 1;
		int start_col = col * pixelsPerCellRow, end_col = (col + 1) * pixelsPerCellRow - 1;
		// initialize all points in that image node
		vector<Point2f> src(pixelsPerCellCol * pixelsPerCellRow);
    	vector<Point2f> dst(pixelsPerCellCol * pixelsPerCellRow);
    	// compute perspective transformation
    	perspectiveTransform( src, dst, H);
    	// store the result (dst - src) into xflow and yflow
    	for (int r = start_row; r <= end_row; r++) {
    		for (int c = start_col; c <= end_col; c++) {
    			int ind = (r - start_row) * pixelsPerCellRow + (c - start_col);
    			xflow.at<xflow.type()>(r,c) = (int)(dst[ind].x - src[ind].x);//is the function called type(), what's its return type?
    			yflow.at<yflow.type()>(r,c) = (int)(dst[ind].y - src[ind].y);//index: first row, then col
    		}
    	}
	}
}
/*********************************************************************************************/

PairedFeaturedImagePyramids::PairedFeaturedImagePyramids(FeaturedImagePyramids ref, FeaturedImagePyramids comp, PairedPyramidHomography PHs) {
	reference = ref;
	companion = comp;
	pairedHomographies = PHs;
	Mat xflow, yflow;
	Mat img = ref.getFeaturedImageByLevel(0).getFeaturedImageNodeByIndex(0).getImage();
	pairedHomographies.discretizeHomography(img.rows, img.cols, xflow, yflow);
	homographyFlow.add(xflow);
	homographyFlow.add(yflow);
}
PairedFeaturedImagePyramids::PairedFeaturedImagePyramids(FeaturedImagePyramids ref, FeaturedImagePyramids comp): pairedHomographies(ref, comp) {
	reference = ref;
	companion = comp;
	Mat xflow, yflow;
	Mat img = ref.getFeaturedImageNodeByIndex(ref.getLevelNumber() - 1).getImage();
	pairedHomographies.discretizeHomography(img.rows, img.cols, xflow, yflow);
	homographyFlow.add(xflow);
	homographyFlow.add(yflow);
}

vector<vector<Mat>> PairedFeaturedImagePyramids::computePyramidHomography() {
	return pairedHomographies.computePyramidHomography(reference, companion);
}

vector<vector<Mat>> PairedFeaturedImagePyramids::getPyramidHomography() {
	return pairedHomographies.getPyramidHomography();
}

vector<Mat> PairedFeaturedImagePyramids::getImageHomographyByLevel(int level) {
	return pairedHomographies.getImageHomographyByLevel(level);
}

Mat PairedFeaturedImagePyramids::getNodeHomographyByLevelAndIndex(int level, int i) {
	return pairedHomographies.getNodeHomographyByLevelAndIndex(level, i);
}