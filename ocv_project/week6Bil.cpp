#include <iostream>
#include "opencv2/core/core.hpp" // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더

using namespace cv;
using namespace std;

double gaussian(float x, double sigma) { //가우시안 함수
	return exp(-(pow(x, 2)) / (2 * pow(sigma, 2))) / (2 * CV_PI * pow(sigma, 2));
}
float distance(int x, int y, int i, int j) { //(x,y)와 (i,j)사이의 거리 계산
	return float(sqrt(pow(x - i, 2) + pow(y - j, 2)));
}

void bilateral(const Mat& src_img, Mat& dst_img, int c, int r, int diameter, 
	double sig_r, double sig_s) {
	int radius = diameter / 2; //윈도우의 반경
	double gr, gs, wei;
	double tmp = 0.;
	double sum = 0.;

	//커널 인덱싱
	for (int kc = -radius; kc <= radius; kc++) {
		for (int kr = -radius; kr <= radius; kr++) {
			
			//range calc
			gr = gaussian(distance(c, r, c + kc, r + kr), sig_r);
			//spatial calc;
			gs = gaussian(src_img.at<uchar>(c, r) - src_img.at<uchar>(c + kc, r + kr), sig_s);
			
			wei = gr * gs; //가중치
			tmp += src_img.at<uchar>(c + kr, r + kr) * wei; //가중치와 픽셀값 곱하기
			sum += wei; //가중치 값 더하기
		}
	}
	dst_img.at<double>(c, r) = tmp / sum; //정규화
}


void myBilateral(const Mat& src_img, Mat& dst_img, int diameter, double sig_r, double sig_s)
{
	dst_img = Mat::zeros(src_img.size(), CV_8UC1); //출력 이미지 초기화

	Mat guide_img = Mat::zeros(src_img.size(), CV_64F); //가이드 이미지 초기화
	int wh = src_img.cols; int hg = src_img.rows;
	int radius = diameter / 2;

	// <픽셀 인덱싱 (가장자리 제외) >
	for (int c = radius + 1; c < hg - radius; c++) {
		for (int r = radius + 1; r < wh - radius; r++) {
			bilateral(src_img, guide_img, c, r, diameter, sig_r, sig_s);
			//화소별 Bilateral 계산 수행
		}

	}
	guide_img.convertTo(dst_img, CV_8UC1); // Mat type변환
}

void doBilateralEx() {
	cout << "--- doBilateralEx() --- \n" << endl;

	//<입력>
	Mat src_img = imread("C:\\DIP_Pic\\6\\rock.png", 0);
	Mat dst_img;
	if (!src_img.data) printf("No image data \n");

	myBilateral(src_img, dst_img, 5, 25.0, 50.0);

	//<출력>
	Mat result_img;
	hconcat(src_img, dst_img, result_img);
	imshow("doBilateralEx()", result_img);
	waitKey(0);

}

int main() {
	// Mat src_img = imread("C:\\DIP_Pic\\6\\salt_pepper.png", 0); // 이미지 읽기
	// int flags = IMREAD_COLOR 또는 int flags = 1 -> 컬러 영상으로 읽음
	// int flags = IMREAD_GRAYSCALE 또는 int flags = 0 -> 흑백 영상으로 읽음
	// int flags = IMREAD_UNCHANGED 또는 int flags = -1 -> 원본 영상의 형식대로 읽음

	doBilateralEx();

	waitKey(0); // 키 입력 대기(0: 키가 입력될 때 까지 프로그램 멈춤)
	destroyWindow("Test window"); // 이미지 출력창 종료
	//imwrite("gear.jpg", doL); // 이미지 쓰기

	return 0;
}