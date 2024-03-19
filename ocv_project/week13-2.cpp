//#include <opencv2/opencv.hpp>
#include <iostream>
#include "opencv2/core/core.hpp" // Mat class�� ���� data structure �� ��� ��ƾ�� �����ϴ� ���
#include "opencv2/highgui/highgui.hpp" // GUI�� ���õ� ��Ҹ� �����ϴ� ���(imshow ��)
#include "opencv2/imgproc/imgproc.hpp" // ���� �̹��� ó�� �Լ��� �����ϴ� ���
#include <vector>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/stitching.hpp.>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/calib3d.hpp>

using namespace cv;
using namespace std;
using namespace cv::xfeatures2d;

Mat makePanorama(Mat img_l, Mat img_r, int thresh_dist, int min_matches) {

	//graysclae�� ��ȯ
	Mat img_gray_l, img_gray_r;
	cvtColor(img_l, img_gray_l, COLOR_BGR2GRAY);
	cvtColor(img_r, img_gray_r, COLOR_BGR2GRAY);

	//Ư¡�� ����
	Ptr<Feature2D> Detector = SIFT::create();
	vector<KeyPoint> kpts_obj, kpts_scene;
	Mat img_des_obj, img_des_scene;
	Detector->detectAndCompute(img_gray_l, cv::Mat(), kpts_obj, img_des_obj);
	Detector->detectAndCompute(img_gray_r, cv::Mat(), kpts_scene, img_des_scene);

	//����� ����
	Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce");
	vector<DMatch> matches;
	matcher->match(img_des_obj, img_des_scene, matches);

	//��Ī ��� �ð�ȭ
	Mat img_matches;
	drawMatches(img_gray_l, kpts_obj, img_gray_r, kpts_scene,
		matches, img_matches, Scalar::all(-1), Scalar::all(-1),
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	imwrite("img_matches.png", img_matches);
	imshow("imgmatches ", img_matches);
	waitKey();

	//��Ī�������
	double dist_max = matches[0].distance;
	double dist_min = matches[0].distance;
	double dist;

	for (int i = 0; i < img_des_obj.rows; i++) {
		dist = matches[i].distance;
		if (dist < dist_min) dist_min = dist;
		if (dist < dist_max) dist_max = dist;
	}

	printf("max_dist: %f \n", dist_max);
	printf("min_dist: %f \n", dist_min);

	vector<DMatch> matches_good;
	do {
		vector<DMatch> good_matches2;
		for (int i = 0; i < img_des_obj.rows; i++) {
			if (matches[i].distance < thresh_dist * dist_min)
				good_matches2.push_back(matches[i]);
		}
		matches_good = good_matches2;
		thresh_dist -= 1;
	} while (thresh_dist != 2 && matches_good.size() > min_matches);

	//��� ��Ī �ð�ȭ
	Mat img_matches_good;
	drawMatches(img_gray_l, kpts_obj, img_gray_r, kpts_scene,
		matches_good, img_matches_good, Scalar::all(-1), Scalar::all(-1),
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	imwrite("img_matches_good.png", img_matches_good);
	imshow("img_matches_good", img_matches_good);
	waitKey();

	//��Ī ��� ��ǥ ����
	vector<Point2f> obj, scene;
	for (int i = 0; i < matches_good.size(); i++) {
		obj.push_back(kpts_obj[matches_good[i].queryIdx].pt); //img1
		scene.push_back(kpts_scene[matches_good[i].trainIdx].pt); //img1

	}

	//��Ī ����κ��� homography ��� ����
	Mat mat_homo = findHomography(scene, obj, RANSAC);
	//�̻�ġ ���Ÿ� ���� RANSAC �߰�

	//homography ����� �̿��� ���� ����ȯ
	Mat img_result;
	warpPerspective(img_r, img_result, mat_homo,
		Size(img_l.cols * 2, img_l.rows * 1.2), INTER_CUBIC);
	//������ �߸��� ���� �����ϱ� ���� ���������� �ο�


	std::vector<Point2f> corners1, corners2;
	//���� �̹����� �� �������� corners1�� ����
	corners1.emplace_back(Point2f(0, 0));
	corners1.emplace_back(Point2f((img_gray_r.cols - 1.f), 0));
	corners1.emplace_back(Point2f(img_gray_r.cols - 1.f, img_gray_r.rows - 1.f));
	corners1.emplace_back(Point2f(0, img_gray_r.rows - 1.f));
	//homography�� ��ȯ�� ������ ��ǥ�� corners2�� ����
	perspectiveTransform(corners1, corners2, mat_homo);

	// ȣ��׷��Ƿ� ��ȯ�� �ڳʸ� �̹����� �׸�
	std::vector<Point> corners_dst;
	for (Point2f pt : corners2) {
		corners_dst.emplace_back(Point(cvRound(pt.x), cvRound(pt.y)));
	}
	// ȣ��׷��Ƿ� ��ȯ�� �κп� �簢�� �׸�
	polylines(img_matches_good, corners_dst, true, Scalar(0, 255, 0), 2, LINE_AA);

	imshow("dst", img_matches_good);
	waitKey();
	destroyAllWindows();

	return img_matches_good;
}

int main() {

	Mat img_r = imread("C:\\DIP_Pic\\13\\Scene.jpg");
	Mat img_l = imread("C:\\DIP_Pic\\13\\Book1.jpg");
	makePanorama(img_r, img_l, 5, 5);
}