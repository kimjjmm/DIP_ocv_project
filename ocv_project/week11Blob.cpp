#include <opencv2/opencv.hpp>
#include <iostream>
#include "opencv2/core/core.hpp" // Mat class�� ���� data structure �� ��� ��ƾ�� �����ϴ� ���
#include "opencv2/highgui/highgui.hpp" // GUI�� ���õ� ��Ҹ� �����ϴ� ���(imshow ��)
#include "opencv2/imgproc/imgproc.hpp" // ���� �̹��� ó�� �Լ��� �����ϴ� ���

using namespace cv;
using namespace std;

void cvBlobDetection() {
	Mat img = imread("C:\\DIP_Pic\\11\\coin.png", IMREAD_COLOR); //�̹��� �б�
	SimpleBlobDetector::Params params; //�Ķ���� ����
	params.minThreshold = 10; //�ּ� �Ӱ谪 ���ϴ� �Ķ����
	params.maxThreshold = 300; //�ִ� �Ӱ谪 ���ϴ� �Ķ����
	params.filterByArea = true; //������ ���� ���͸� ����
	params.minArea = 10; //�ּҸ��� �Ķ����
	params.maxArea = 7000; //�ִ���� �Ķ����
	params.filterByCircularity = true; //��ü�� ����ġ ���͸� ���� �����ϴ� �Ķ����
	params.minCircularity = 0.5; //�ּ� ����ġ ���� �Ķ����
	params.filterByConvexity = true; //��ü�� �����Կ� ���� ���͸� ���� �����ϴ� �Ķ����
	params.minConvexity = 0.9; //�ּ� ������ �Ķ����
	params.filterByInertia = true; //Ÿ���� ���� �󸶳� ������� ����
	params.minInertiaRatio = 0.9; //�ּ� Ÿ��/�� ���� ����

	Ptr<SimpleBlobDetector>detector = SimpleBlobDetector::create(params); //create�� parameter�� ����

	//keypoint�� ����� ������ ����� ����
	std::vector<KeyPoint> keypoints; 
	detector->detect(img, keypoints); 

	Mat result; 
	//�̹����� keypoints �׸���
	drawKeypoints(img, keypoints, result, Scalar(255, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

	cout << "������ ����: " << keypoints.size() << endl; //���� ���� ���

	imshow("keypoints", result); //�̹��� ���
	waitKey(0);
	destroyAllWindows();
}

int main() {
	cvBlobDetection();
}