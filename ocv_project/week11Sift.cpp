#include <opencv2/opencv.hpp>
#include <iostream>
#include "opencv2/core/core.hpp" // Mat class�� ���� data structure �� ��� ��ƾ�� �����ϴ� ���
#include "opencv2/highgui/highgui.hpp" // GUI�� ���õ� ��Ҹ� �����ϴ� ���(imshow ��)
#include "opencv2/imgproc/imgproc.hpp" // ���� �̹��� ó�� �Լ��� �����ϴ� ���

using namespace cv;
using namespace std;

void cvFeatureSIFT() {
	Mat img = imread("C:\\DIP_Pic\\11\\church.jpg", 1); //�̹��� �б�

	Mat gray;
	cvtColor(img, gray, CV_BGR2GRAY); //�÷� �̹����� �׷��̽����Ϸ� ��ȯ

	Ptr<SiftFeatureDetector> detector = SiftFeatureDetector::create(); //sift����� ����
	vector<KeyPoint>keypoints;
	detector->detect(gray, keypoints); //sift Ű����Ʈ(�̹������� Ư¡���� ��ġ) ����

	Mat result;
    //�����̹��� ���� Ű����Ʈ�� �׸�
	drawKeypoints(img, keypoints, result, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

	//imwrite("sift_result.jpg", result);
	imshow("Sift result", result); //�̹��� ���
	waitKey(0);
	cvDestroyAllWindows();
}

void warpPersSift() {
    Mat src = imread("C:\\DIP_Pic\\11\\church.jpg", 1); //�̹��� �б�
    Mat dst;

    Point2f src_p[4], dst_p[4]; //��ȯ ��, �� ������ ����

    //��ȯ �� ������
    src_p[0] = Point2f(0, 0);
    src_p[1] = Point2f(512, 0);
    src_p[2] = Point2f(0, 512);
    src_p[3] = Point2f(512, 512);
    //cout << src.size()<<endl;

    //��ȯ �� ������
    dst_p[0] = Point2f(0, 0);
    dst_p[1] = Point2f(512, 0);
    dst_p[2] = Point2f(0, 512);
    dst_p[3] = Point2f(412, 412);

    Mat pers_mat = getPerspectiveTransform(src_p, dst_p);//���ٺ�ȯ ��� ����
    warpPerspective(src, dst, pers_mat, src.size());//���ٺ�ȯ. 
    imshow("warpResult", dst); //�̹��� ���

    //��� ����
    Mat hsv;
    cvtColor(dst, hsv, COLOR_BGR2HSV); //�̹����� HSV���������� ��ȯ
    vector<Mat> channels;
    split(hsv, channels);//HSV�̹����� ä�η� �и�
    channels[2] += 100; //���ä���� 100��ŭ ��� ����
    merge(channels, hsv);//�и��� ä�� �ٽ� ����
    cvtColor(hsv, dst, COLOR_HSV2BGR); //HSV�� BGR�� ��ȯ

    Mat gray;
    cvtColor(dst, gray, CV_BGR2GRAY); //�÷� �̹����� �׷��̽����Ϸ� ��ȯ

	Ptr<SiftFeatureDetector> detector = SiftFeatureDetector::create();
	vector<KeyPoint>keypoints;
	detector->detect(gray, keypoints); //sift Ű����Ʈ(�̹������� Ư¡���� ��ġ) ����

	Mat result;
    //�����̹��� ���� Ű����Ʈ�� �׸�

	drawKeypoints(dst, keypoints, result, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    cout << result.size() << endl;

	//imwrite("warpsift_result.jpg", result);
	imshow("Sift result", result); //�̹��� ���
	waitKey(0);
	cvDestroyAllWindows();

}

int main() {
    cvFeatureSIFT();
    warpPersSift();
}