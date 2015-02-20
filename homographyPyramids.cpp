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

/*
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
};
*/

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

vector<Mat> getNeighborHomographies(int level, int i) {
	return pairedImageHomographies[level].getNeighborHomographies(i);
}

Mat getUpperLevelHomography(int level, int i) {
	int row, col;
	int colsPerRow = pairedImageHomographies[level].getColsPerRow();
	rowColumnOfIndex(i, colsPerRow, row, col);
	return pairedImageHomographies[level].getNodeHomographyByIndex(indexOfRowColumn(row / 2, col / 2, colsPerRow));
}

vector<vector<Mat>> refineHomographies() {
	//
}
/*********************************************************************************************/
/*
class PairedFeaturedImagePyramids {
protected:
	FeaturedImagePyramids reference;
	FeaturedImagePyramids companion;
	PairedPyramidHomography pairedHomographies;
public:
	PairedFeaturedImagePyramids(FeaturedImagePyramids ref, FeaturedImagePyramids comp, PairedPyramidHomography PHs);
	PairedFeaturedImagePyramids(FeaturedImagePyramids ref, FeaturedImagePyramids comp);
	vector<vector<Mat>> computePyramidHomography();
	vector<vector<Mat>> getPyramidHomography();
	vector<Mat> getImageHomographyByLevel(int level);
	Mat getNodeHomographyByLevelAndIndex(int level, int i);
};
*/

PairedFeaturedImagePyramids::PairedFeaturedImagePyramids(FeaturedImagePyramids ref, FeaturedImagePyramids comp, PairedPyramidHomography PHs) {
	reference = ref;
	companion = comp;
	pairedHomographies = PHs;
}
PairedFeaturedImagePyramids::PairedFeaturedImagePyramids(FeaturedImagePyramids ref, FeaturedImagePyramids comp) {
	//
}

vector<vector<Mat>> PairedFeaturedImagePyramids::computePyramidHomography() {
	int levelNum = ref.getLevelNumber();
	for (int l = 0; l < levelNum; l++) {
		FeaturedImage srcImage = reference.getFeaturedImageByLevel(l);
		FeaturedImage dstImage = companion.getFeaturedImageByLevel(l);
		PairedPyramidHomography pairedHomography
	}
}

vector<vector<Mat>> PairedFeaturedImagePyramids::getPyramidHomography() {
	//
}

vector<Mat> PairedFeaturedImagePyramids::getImageHomographyByLevel(int level) {
	//
}

Mat PairedFeaturedImagePyramids::getNodeHomographyByLevelAndIndex(int level, int i) {
	//
}