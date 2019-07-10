#include <stdio.h>
#include <iostream>
#include "opencv2/opencv.hpp"

using namespace cv;
//using namespace cv::xfeatures2d;

//void readme();

#define RATIO    0.75

void matchFeatures(const cv::Mat &query, const cv::Mat &target,
	std::vector<cv::DMatch> &goodMatches) {
	std::vector<std::vector<cv::DMatch>> matches;
	cv::Ptr<cv::BFMatcher> matcher = cv::BFMatcher::create();
	// Find 2 best matches for each descriptor to make later the second neighbor test.
	std::cout << query;
	std::cout << target;

	matcher->knnMatch(query, target, matches, 2);
	// Second neighbor ratio test.
	for (unsigned int i = 0; i < matches.size(); ++i) {
		if (matches[i][0].distance < matches[i][1].distance * RATIO)
			goodMatches.push_back(matches[i][0]);
	}
}

/** @function main */
int main(int argc, char** argv)
{
	//if (argc != 3)
	//{
	//	readme(); return -1;
	//}

	Mat img_1 = imread("C:/Users/temp2019/Desktop/nizhniy-800/data/1.jpg", IMREAD_GRAYSCALE);
	Mat img_2 = imread("C:/Users/temp2019/Desktop/nizhniy-800/data/2.jpg", IMREAD_GRAYSCALE);

	if (!img_1.data || !img_2.data)
	{
		std::cout << " --(!) Error reading images " << std::endl; return -1;
	}

	//-- Step 1: Detect the keypoints using SURF Detector
	int minHessian = 400;

	Ptr<ORB> detector = ORB::create();

	std::vector<KeyPoint> keypoints_1, keypoints_2;

	detector->detect(img_1, keypoints_1);
	detector->detect(img_2, keypoints_2);

	//-- Draw keypoints
	Mat img_keypoints_1; Mat img_keypoints_2;

	drawKeypoints(img_1, keypoints_1, img_keypoints_1, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
	drawKeypoints(img_2, keypoints_2, img_keypoints_2, Scalar::all(-1), DrawMatchesFlags::DEFAULT);

	//-- Show detected (drawn) keypoints
	imshow("Keypoints 1", img_keypoints_1);
	imshow("Keypoints 2", img_keypoints_2);
	
	///////////////////////////////////////////////////////////////////

	std::vector<cv::KeyPoint> keyPoints1, keyPoints2;
	cv::Mat descriptors1, descriptors2;
	//cv::Ptr<cv::ORB> detector = cv::ORB::create();
	detector->detectAndCompute(img_1, cv::Mat(), keyPoints1, descriptors1);
	detector->detectAndCompute(img_2, cv::Mat(), keyPoints2, descriptors2);
	// Match features.
	std::vector<cv::DMatch> matches;


	matchFeatures(descriptors1, descriptors2, matches);
	// Draw matches.
	cv::Mat image_matches;
	cv::drawMatches(img_1, keypoints_1, img_2, keypoints_2, matches, image_matches);
	cv::imshow("Matches", image_matches);

	waitKey(0);

	return 0;
}

/** @function readme */
void readme()
{
	std::cout << " Usage: ./SURF_detector <img1> <img2>" << std::endl;
}