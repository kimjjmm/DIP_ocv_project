//#include <opencv2/opencv.hpp>
#include <iostream>
#include "opencv2/core/core.hpp" // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더
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

	//graysclae로 변환
	Mat img_gray_l, img_gray_r;
	cvtColor(img_l, img_gray_l, COLOR_BGR2GRAY);
	cvtColor(img_r, img_gray_r, COLOR_BGR2GRAY);

	//특징점 추출
	Ptr<Feature2D> Detector = SIFT::create();
	vector<KeyPoint> kpts_obj, kpts_scene;
	Mat img_des_obj, img_des_scene;
	Detector->detectAndCompute(img_gray_l, cv::Mat(), kpts_obj, img_des_obj);
	Detector->detectAndCompute(img_gray_r, cv::Mat(), kpts_scene, img_des_scene);

	//기술자 추출
	Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce");
	vector<DMatch> matches;
	matcher->match(img_des_obj, img_des_scene, matches);

	//매칭 결과 시각화
	Mat img_matches;
	drawMatches(img_gray_l, kpts_obj, img_gray_r, kpts_scene,
		matches, img_matches, Scalar::all(-1), Scalar::all(-1),
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	imwrite("img_matches.png", img_matches);
	imshow("imgmatches ", img_matches);
	waitKey();

	//매칭결과정제
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

	//우수 매칭 시각화
	Mat img_matches_good;
	drawMatches(img_gray_l, kpts_obj, img_gray_r, kpts_scene,
		matches_good, img_matches_good, Scalar::all(-1), Scalar::all(-1),
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	imwrite("img_matches_good.png", img_matches_good);
	imshow("img_matches_good", img_matches_good);
	waitKey();

	//매칭 결과 좌표 추출
	vector<Point2f> obj, scene;
	for (int i = 0; i < matches_good.size(); i++) {
		obj.push_back(kpts_obj[matches_good[i].queryIdx].pt); //img1
		scene.push_back(kpts_scene[matches_good[i].trainIdx].pt); //img1

	}

	//매칭 결과로부터 homography 행렬 추출
	Mat mat_homo = findHomography(scene, obj, RANSAC);
	//이상치 제거를 위해 RANSAC 추가

	//homography 행렬을 이용해 시점 역변환
	Mat img_result;
	warpPerspective(img_r, img_result, mat_homo,
		Size(img_l.cols * 2, img_l.rows * 1.2), INTER_CUBIC);
	//영상이 잘리는 것을 방지하기 위해 여유공간을 부여


	std::vector<Point2f> corners1, corners2;
	//원본 이미지의 네 꼭짓점을 corners1에 저장
	corners1.emplace_back(Point2f(0, 0));
	corners1.emplace_back(Point2f((img_gray_r.cols - 1.f), 0));
	corners1.emplace_back(Point2f(img_gray_r.cols - 1.f, img_gray_r.rows - 1.f));
	corners1.emplace_back(Point2f(0, img_gray_r.rows - 1.f));
	//homography로 변환된 꼭짓점 좌표를 corners2에 저장
	perspectiveTransform(corners1, corners2, mat_homo);

	// 호모그래피로 변환된 코너를 이미지에 그림
	std::vector<Point> corners_dst;
	for (Point2f pt : corners2) {
		corners_dst.emplace_back(Point(cvRound(pt.x), cvRound(pt.y)));
	}
	// 호모그래피로 변환된 부분에 사각형 그림
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