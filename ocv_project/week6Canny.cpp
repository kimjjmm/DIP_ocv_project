#include <iostream>
#include "opencv2/core/core.hpp" // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더

using namespace cv;
using namespace std;

void doCannyEx() {
	clock_t start, end; // clock_t 타입 변수 선언
	start = clock(); // 시작 시간 기록
	cout << "--- doCannyEx() ---\n" << endl;
	Mat src_img = imread("C:\\DIP_Pic\\6\\edge_test.jpg", 0);
	if (!src_img.data) printf("No image data \n");

	Mat dst_img;
	
	Canny(src_img, dst_img, 500, 600);
	end = clock(); // 종료 시간 기록
	double time_taken = double(end - start) / double(CLOCKS_PER_SEC); // 실행 시간 계산

	cout << "실행 시간 : " << time_taken << "초" << endl;
	Mat result_img;
	hconcat(src_img, dst_img, result_img);

	imshow("doCannyEx()", result_img);
	waitKey(0);
}
int main() {
	// Mat src_img = imread("C:\\DIP_Pic\\6\\salt_pepper.png", 0); // 이미지 읽기
	// int flags = IMREAD_COLOR 또는 int flags = 1 -> 컬러 영상으로 읽음
	// int flags = IMREAD_GRAYSCALE 또는 int flags = 0 -> 흑백 영상으로 읽음
	// int flags = IMREAD_UNCHANGED 또는 int flags = -1 -> 원본 영상의 형식대로 읽음

	doCannyEx();

	return 0;
}