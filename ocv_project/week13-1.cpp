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

void ex_panorama_simple() {

	Mat img;
	vector<Mat>imgs;
	//�̹��� �а� ���Ϳ� �߰�
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
	//��ƼĪ ��ü ����
	Ptr<Stitcher> stitcher = Stitcher::create(Stitcher::PANORAMA, false);

	//�̹��� ��ƼĪ
	Stitcher::Status status = stitcher->stitch(imgs, result);
	//����ó��. ��ƼĪ�� ���� ���� ��� 
	if (status != Stitcher::OK) {
		cout << "Can't stitch images, error code = " << int(status) << endl;
		exit(-1);
	}

	imshow("ex_panorama_simple_result", result);//�̹��� ���
	imwrite("ex_panorama_simple_result.png", result);
	waitKey();
}

Mat makePanorama(Mat img_l, Mat img_r, int thresh_dist, int min_matches) {

	//graysclae�� ��ȯ
	Mat img_gray_l, img_gray_r;
	cvtColor(img_l, img_gray_l, CV_BGR2GRAY);
	cvtColor(img_r, img_gray_r, CV_BGR2GRAY);

	//Ư¡�� ����
	Ptr<SurfFeatureDetector> Detector = SURF::create(300);
	vector<KeyPoint>kpts_obj, kpts_scene;
	Detector->detect(img_gray_l, kpts_obj);
	Detector->detect(img_gray_r, kpts_scene);

	//Ư¡�� �ð�ȭ
	Mat img_kpts_l, img_kpts_r;
	drawKeypoints(img_gray_l, kpts_obj, img_kpts_l,
		Scalar::all(-1), DrawMatchesFlags::DEFAULT);
	drawKeypoints(img_gray_r, kpts_scene, img_kpts_r,
		Scalar::all(-1), DrawMatchesFlags::DEFAULT);
	imwrite("img_kpts_l.png", img_kpts_l);
	imwrite("img_kpts_r.png", img_kpts_r);

	//����� ����
	Ptr<SurfDescriptorExtractor>Extractor =
		SURF::create(100, 4, 3, false, true);
	Mat img_des_obj, img_des_scene;
	Extractor->compute(img_gray_l, kpts_obj, img_des_obj);
	Extractor->compute(img_gray_r, kpts_scene, img_des_scene);

	//����ڸ� �̿��� Ư¡�� ��Ī
	BFMatcher matcher(NORM_L2);
	vector<DMatch> matches;
	matcher.match(img_des_obj, img_des_scene, matches);

	//��Ī ��� �ð�ȭ
	Mat img_matches;
	drawMatches(img_gray_l, kpts_obj, img_gray_r, kpts_scene,
		matches, img_matches, Scalar::all(-1), Scalar::all(-1),
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	imwrite("img_matches.png", img_matches);

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

	//��� ��Ī ��� �ð�ȭ
	Mat img_matches_good;
	drawMatches(img_gray_l, kpts_obj, img_gray_r, kpts_scene,
		matches_good, img_matches_good, Scalar::all(-1), Scalar::all(-1),
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	imwrite("img_matches_good.png", img_matches_good);


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
	// 
	//���� ����� ����ȯ�� ���� ���� ��ü
	Mat img_pano;
	img_pano = img_result.clone();
	Mat roi(img_pano, Rect(0, 0, img_l.cols, img_l.rows));
	img_l.copyTo(roi);

	//���� ���� �߶󳻱�
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
	//�̹��� �б�
	Mat matImage1 = imread("C:\\DIP_Pic\\13\\cen.jpg", IMREAD_COLOR);
	Mat matImage2 = imread("C:\\DIP_Pic\\13\\le.jpg", IMREAD_COLOR);
	Mat matImage3 = imread("C:\\DIP_Pic\\13\\ri.jpg", IMREAD_COLOR);
	//�̹���ũ�� ���� 512X512
	resize(matImage1, matImage1, Size(512, 512), 0, 0, INTER_CUBIC);
	resize(matImage2, matImage2, Size(512, 512), 0, 0, INTER_CUBIC);
	resize(matImage3, matImage3, Size(512, 512), 0, 0, INTER_CUBIC);

	//�̹����� ����� ������ ���� ��� ���α׷� ����
	if (matImage1.empty() || matImage2.empty() || matImage3.empty()) exit(-1);

	Mat result;
	//����������� �̹��� flip
	flip(matImage1, matImage1, 1);
	flip(matImage2, matImage2, 1);

	//�ĳ�� �̹��� ����
	result = makePanorama(matImage1, matImage2, 3, 60);
	
	flip(result, result, 1); 	//����������� �̹��� flip
	
	//���������� �ĳ�� �̹��� ����
	result = makePanorama(result, matImage3, 3, 60);

	imshow("ex_panorama_result", result); //�̹��� ���
	imwrite("ex_panorama_result.png", result);
	waitKey();
}

int main() {
	ex_panorama_simple();
	ex_panorama(); //�Լ� ����
}