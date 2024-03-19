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

void ex_panorama_simple() {

	Mat img;
	vector<Mat>imgs;
	//이미지 읽고 벡터에 추가
	img = imread("C:\\DIP_Pic\\13\\le.jpg", IMREAD_COLOR);
	resize(img, img, Size(512, 512), 0, 0, INTER_CUBIC);
	imgs.push_back(img);
	img = imread("C:\\DIP_Pic\\13\\cen.jpg", IMREAD_COLOR);
	resize(img, img, Size(512, 512), 0, 0, INTER_CUBIC);
	imgs.push_back(img);
	img = imread("C:\\DIP_Pic\\13\\ri.jpg", IMREAD_COLOR);
	resize(img, img, Size(512, 512), 0, 0, INTER_CUBIC);
	imgs.push_back(img);

	Mat result;
	//스티칭 객체 생성
	Ptr<Stitcher> stitcher = Stitcher::create(Stitcher::PANORAMA, false);

	//이미지 스티칭
	Stitcher::Status status = stitcher->stitch(imgs, result);
	//예외처리. 스티칭이 되지 않은 경우 
	if (status != Stitcher::OK) {
		cout << "Can't stitch images, error code = " << int(status) << endl;
		exit(-1);
	}

	imshow("ex_panorama_simple_result", result);//이미지 출력
	imwrite("ex_panorama_simple_result.png", result);
	waitKey();
}

Mat makePanorama(Mat img_l, Mat img_r, int thresh_dist, int min_matches) {

	//graysclae로 변환
	Mat img_gray_l, img_gray_r;
	cvtColor(img_l, img_gray_l, CV_BGR2GRAY);
	cvtColor(img_r, img_gray_r, CV_BGR2GRAY);

	//특징점 추출
	Ptr<SurfFeatureDetector> Detector = SURF::create(300);
	vector<KeyPoint>kpts_obj, kpts_scene;
	Detector->detect(img_gray_l, kpts_obj);
	Detector->detect(img_gray_r, kpts_scene);

	//특징점 시각화
	Mat img_kpts_l, img_kpts_r;
	drawKeypoints(img_gray_l, kpts_obj, img_kpts_l,
		Scalar::all(-1), DrawMatchesFlags::DEFAULT);
	drawKeypoints(img_gray_r, kpts_scene, img_kpts_r,
		Scalar::all(-1), DrawMatchesFlags::DEFAULT);
	imwrite("img_kpts_l.png", img_kpts_l);
	imwrite("img_kpts_r.png", img_kpts_r);

	//기술자 추출
	Ptr<SurfDescriptorExtractor>Extractor =
		SURF::create(100, 4, 3, false, true);
	Mat img_des_obj, img_des_scene;
	Extractor->compute(img_gray_l, kpts_obj, img_des_obj);
	Extractor->compute(img_gray_r, kpts_scene, img_des_scene);

	//기술자를 이용한 특징점 매칭
	BFMatcher matcher(NORM_L2);
	vector<DMatch> matches;
	matcher.match(img_des_obj, img_des_scene, matches);

	//매칭 결과 시각화
	Mat img_matches;
	drawMatches(img_gray_l, kpts_obj, img_gray_r, kpts_scene,
		matches, img_matches, Scalar::all(-1), Scalar::all(-1),
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	imwrite("img_matches.png", img_matches);

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

	//우수 매칭 결과 시각화
	Mat img_matches_good;
	drawMatches(img_gray_l, kpts_obj, img_gray_r, kpts_scene,
		matches_good, img_matches_good, Scalar::all(-1), Scalar::all(-1),
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	imwrite("img_matches_good.png", img_matches_good);


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
	// 
	//기준 영상과 역변환된 시점 영상 합체
	Mat img_pano;
	img_pano = img_result.clone();
	Mat roi(img_pano, Rect(0, 0, img_l.cols, img_l.rows));
	img_l.copyTo(roi);

	//검은 여백 잘라내기
	int cut_x = 0, cut_y = 0;
	for (int y = 0; y < img_pano.rows; y++) {
		for (int x = 0; x < img_pano.cols; x++)
		{
			if (img_pano.at<Vec3b>(y, x)[0] == 0 && img_pano.at<Vec3b>(y, x)[1] == 0 && img_pano.at<Vec3b>(y, x)[2] == 0)
			{
				continue;
			}
			if (cut_x < x) cut_x = x;
			if (cut_y < y) cut_y = y;
		}
	}
	Mat img_pano_cut;
	img_pano_cut = img_pano(Range(0, cut_y), Range(0, cut_x));
	imwrite("img_pano_cut.png", img_pano_cut);

	return img_pano_cut;
}

void ex_panorama() {
	//이미지 읽기
	Mat matImage1 = imread("C:\\DIP_Pic\\13\\cen.jpg", IMREAD_COLOR);
	Mat matImage2 = imread("C:\\DIP_Pic\\13\\le.jpg", IMREAD_COLOR);
	Mat matImage3 = imread("C:\\DIP_Pic\\13\\ri.jpg", IMREAD_COLOR);
	//이미지크기 조정 512X512
	resize(matImage1, matImage1, Size(512, 512), 0, 0, INTER_CUBIC);
	resize(matImage2, matImage2, Size(512, 512), 0, 0, INTER_CUBIC);
	resize(matImage3, matImage3, Size(512, 512), 0, 0, INTER_CUBIC);

	//이미지가 제대로 읽히지 않은 경우 프로그램 종료
	if (matImage1.empty() || matImage2.empty() || matImage3.empty()) exit(-1);

	Mat result;
	//수평방향으로 이미지 flip
	flip(matImage1, matImage1, 1);
	flip(matImage2, matImage2, 1);

	//파노라마 이미지 생성
	result = makePanorama(matImage1, matImage2, 3, 60);
	
	flip(result, result, 1); 	//수평방향으로 이미지 flip
	
	//최종적으로 파노라마 이미지 생성
	result = makePanorama(result, matImage3, 3, 60);

	imshow("ex_panorama_result", result); //이미지 출력
	imwrite("ex_panorama_result.png", result);
	waitKey();
}

int main() {
	ex_panorama_simple();
	ex_panorama(); //함수 실행
}