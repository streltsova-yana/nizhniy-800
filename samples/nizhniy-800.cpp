//#include <stdio.h>
//#include <iostream>
//#include "opencv2/opencv.hpp"
//
//using namespace cv;
////using namespace cv::xfeatures2d;
//
////void readme();
//
//#define RATIO    0.75
//
//void matchFeatures(const cv::Mat & query, const cv::Mat & target,
//	std::vector<cv::DMatch> & goodMatches) {
//	std::vector<std::vector<cv::DMatch>> matches;
//	cv::Ptr<cv::BFMatcher> matcher = cv::BFMatcher::create();
//	// Find 2 best matches for each descriptor to make later the second neighbor test.
//	std::cout << query;
//	std::cout << target;
//
//	matcher->knnMatch(query, target, matches, 2);
//	// Second neighbor ratio test.
//	for (unsigned int i = 0; i < matches.size(); ++i) {
//		if (matches[i][0].distance < matches[i][1].distance * RATIO)
//			goodMatches.push_back(matches[i][0]);
//	}
//}
//
///** @function main */
//int main(int argc, char** argv)
//{
//	//if (argc != 3)
//	//{
//	//	readme(); return -1;
//	//}
//
//	Mat img_1 = imread("C:/Users/яна/Desktop/Project/3.jpg", IMREAD_GRAYSCALE);
//	Mat img_2 = imread("C:/Users/яна/Desktop/Project/5.jpg", IMREAD_GRAYSCALE);
//	resize(img_1, img_1, Size(500, 500));
//	resize(img_2, img_2, Size(500, 500));
//	if (!img_1.data || !img_2.data)
//	{
//		std::cout << " --(!) Error reading images " << std::endl; return -1;
//	}
//
//	//-- Step 1: Detect the keypoints using SURF Detector
//	int minHessian = 400;
//
//	Ptr<ORB> detector = ORB::create();
//
//	std::vector<KeyPoint> keypoints_1, keypoints_2;
//
//	detector->detect(img_1, keypoints_1);
//	detector->detect(img_2, keypoints_2);
//
//	//-- Draw keypoints
//	Mat img_keypoints_1; Mat img_keypoints_2;
//
//	drawKeypoints(img_1, keypoints_1, img_keypoints_1, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
//	drawKeypoints(img_2, keypoints_2, img_keypoints_2, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
//
//	//-- Show detected (drawn) keypoints
//	/*imshow("Keypoints 1", img_keypoints_1);
//	imshow("Keypoints 2", img_keypoints_2);*/
//
//	///////////////////////////////////////////////////////////////////
//
//	std::vector<cv::KeyPoint> keyPoints1, keyPoints2;
//	cv::Mat descriptors1, descriptors2;
//	//cv::Ptr<cv::ORB> detector = cv::ORB::create();
//	detector->detectAndCompute(img_1, cv::Mat(), keyPoints1, descriptors1);
//	detector->detectAndCompute(img_2, cv::Mat(), keyPoints2, descriptors2);
//	// Match features.
//	std::vector<cv::DMatch> matches;
//
//
//	matchFeatures(descriptors1, descriptors2, matches);
//	// Draw matches.
//	cv::Mat image_matches;
//	cv::drawMatches(img_1, keypoints_1, img_2, keypoints_2, matches, image_matches);
//	cv::imshow("Matches", image_matches);
//
//	waitKey(0);
//
//	return 0;
//}
//
///** @function readme */
//void readme()
//{
//	std::cout << " Usage: ./SURF_detector <img1> <img2>" << std::endl;
//}
#include <stdio.h>
#include <iostream>
#include <opencv2/core/types_c.h>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/calib3d.hpp"
//#include "opencv2/xfeatures2d.hpp"
using namespace cv;
//using namespace cv::xfeatures2d;
void readme();
/* @function main */
int main(int argc, char** argv)
{
	if (argc != 3)
	{
		readme(); return -1;
	}
	Mat img_object = imread(argv[1], IMREAD_GRAYSCALE);
	Mat img_scene = imread(argv[2], IMREAD_GRAYSCALE);
	resize(img_object, img_object, Size(500, 500));
	resize(img_scene, img_scene, Size(500, 500));
	if (!img_object.data || !img_scene.data)
	{
		std::cout << " --(!) Error reading images " << std::endl; return -1;
	}
	//-- Step 1: Detect the keypoints and extract descriptors using SURF
	int minHessian = 400;
	Ptr<ORB> detector = ORB::create(minHessian);
	std::vector<KeyPoint> keypoints_object, keypoints_scene;
	Mat descriptors_object, descriptors_scene;
	detector->detectAndCompute(img_object, Mat(), keypoints_object, descriptors_object);
	detector->detectAndCompute(img_scene, Mat(), keypoints_scene, descriptors_scene);
	//-- Step 2: Matching descriptor vectors using  BFmatcher
	BFMatcher matcher;
	std::vector< DMatch > matches;
	matcher.match(descriptors_object, descriptors_scene, matches);
	double max_dist = 0; double min_dist = 100;
	//-- Quick calculation of max and min distances between keypoints
	for (int i = 0; i < descriptors_object.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}
	printf("-- Max dist : %f \n", max_dist);
	printf("-- Min dist : %f \n", min_dist);
	//-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
	std::vector< DMatch > good_matches;
	for (int i = 0; i < descriptors_object.rows; i++)
	{
		if (matches[i].distance <= 2.8 * min_dist)
		{
			good_matches.push_back(matches[i]);
		}
	}
	Mat img_matches;
	drawMatches(img_object, keypoints_object, img_scene, keypoints_scene,
		good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
		std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	//-- Localize the object
	std::vector<Point2f> obj;
	std::vector<Point2f> scene;
	for (size_t i = 0; i < good_matches.size(); i++)
	{
		//-- Get the keypoints from the good matches
		obj.push_back(keypoints_object[good_matches[i].queryIdx].pt);
		scene.push_back(keypoints_scene[good_matches[i].trainIdx].pt);
	}
	Mat H = findHomography(obj, scene, RANSAC);
	//-- Get the corners from the image_1 ( the object to be "detected" )
	std::vector<Point2f> obj_corners(4);
	obj_corners[0] = cvPoint(0, 0); obj_corners[1] = cvPoint(img_object.cols, 0);
	obj_corners[2] = cvPoint(img_object.cols, img_object.rows); obj_corners[3] = cvPoint(0, img_object.rows);
	std::vector<Point2f> scene_corners(4);
	perspectiveTransform(obj_corners, scene_corners, H);
	//-- Draw lines between the corners (the mapped object in the scene - image_2 )
	line(img_matches, scene_corners[0] + Point2f(img_object.cols, 0), scene_corners[1] + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);
	line(img_matches, scene_corners[1] + Point2f(img_object.cols, 0), scene_corners[2] + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);
	line(img_matches, scene_corners[2] + Point2f(img_object.cols, 0), scene_corners[3] + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);
	line(img_matches, scene_corners[3] + Point2f(img_object.cols, 0), scene_corners[0] + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);
	//-- Show detected matches
	imshow("Good Matches & Object detection", img_matches);
	waitKey(0);
	return 0;
}
/* @function readme */
void readme()
{
	std::cout << " Usage: ./SURF_descriptor <img1> <img2>" << std::endl;
}