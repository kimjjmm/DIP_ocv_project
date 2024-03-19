#include <opencv2/opencv.hpp>
#include <iostream>
#include "opencv2/core/core.hpp" // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더
using namespace cv;
using namespace std;


void GrabCut(Mat img, int x1, int y1, int x2, int y2) {
	Rect rect = Rect(Point(x1, y1), Point(x2, y2)); //x,y좌표로 직사각형 영역을 추출
	Mat result, bg_model, fg_model;

	grabCut(img, result, rect, bg_model, fg_model, 5, GC_INIT_WITH_RECT);
	imshow("result", result); //이미지 출력
	//waitKey();

	compare(result, GC_PR_FGD, result, CMP_EQ);
	//GC_PR_FGD: GrabCut class foreground 픽셀
	//CMP_EQ: compare옵션 (equal)

	imshow("result", result); //이미지 출력
	//waitKey();

	Mat mask(img.size(), CV_8UC3, cv::Scalar(255, 255, 255)); //마스크 생성. 픽셀 흰색으로 초기화
	img.copyTo(mask, result); //전경부분 복사

	rectangle(img, Rect(Point(x1, y1), Point(x2, y2)), Scalar(0, 0, 255), 2, 8, 0);
	//원본이미지에 선택한 영역을 빨간색 사각형 그리기

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