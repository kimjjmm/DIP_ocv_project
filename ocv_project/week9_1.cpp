#include <opencv2/opencv.hpp>
#include <iostream>
#include "opencv2/core/core.hpp" // Mat class�� ���� data structure �� ��� ��ƾ�� �����ϴ� ���
#include "opencv2/highgui/highgui.hpp" // GUI�� ���õ� ��Ҹ� �����ϴ� ���(imshow ��)
#include "opencv2/imgproc/imgproc.hpp" // ���� �̹��� ó�� �Լ��� �����ϴ� ���
using namespace cv;
using namespace std;


void GrabCut(Mat img, int x1, int y1, int x2, int y2) {
	Rect rect = Rect(Point(x1, y1), Point(x2, y2)); //x,y��ǥ�� ���簢�� ������ ����
	Mat result, bg_model, fg_model;

	grabCut(img, result, rect, bg_model, fg_model, 5, GC_INIT_WITH_RECT);
	imshow("result", result); //�̹��� ���
	//waitKey();

	compare(result, GC_PR_FGD, result, CMP_EQ);
	//GC_PR_FGD: GrabCut class foreground �ȼ�
	//CMP_EQ: compare�ɼ� (equal)

	imshow("result", result); //�̹��� ���
	//waitKey();

	Mat mask(img.size(), CV_8UC3, cv::Scalar(255, 255, 255)); //����ũ ����. �ȼ� ������� �ʱ�ȭ
	img.copyTo(mask, result); //����κ� ����

	rectangle(img, Rect(Point(x1, y1), Point(x2, y2)), Scalar(0, 0, 255), 2, 8, 0);
	//�����̹����� ������ ������ ������ �簢�� �׸���

	imshow("mask", mask);
	imshow("img", img);
	imshow("result", result);
	waitKey();
	destroyAllWindows();
}

int main() {
	//Mat src_img = imread("C:\\DIP_Pic\\9\\hamster.jpg", 1);
	//int x1 = 300;
	//int y1 = 80;
	//int x2 = 800;
	//int y2 = 500;
	//GrabCut(src_img, x1, y1, x2, y2);

	//Mat src_img2 = imread("C:\\DIP_Pic\\9\\dog.jpg", 1);
	//int x1 = 100;
	//int y1 = 70;
	//int x2 = 800;
	//int y2 = 500;
	//GrabCut(src_img2, x1, y1, x2, y2);

	Mat src_img2 = imread("C:\\DIP_Pic\\9\\snowdog.jpg", 1);
	int x1 = 200;
	int y1 = 40;
	int x2 = 500;
	int y2 = 400;
	GrabCut(src_img2, x1, y1, x2, y2);

	

	return 0;
}