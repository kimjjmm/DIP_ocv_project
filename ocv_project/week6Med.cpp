#include <iostream>
#include "opencv2/core/core.hpp" // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더

using namespace cv;
using namespace std;

void myMedian(const Mat& src_img, Mat& dst_img, const Size& kn_size) {
	dst_img = Mat::zeros(src_img.size(), CV_8UC1);
	int wd = src_img.cols; //입력이미지 너비로 초기화
	int hg = src_img.rows; //입력이미지 높이로 초기화
	int kwd = kn_size.width;  //커널너비 초기화
	int khg = kn_size.height; //커널높이 초기화
	int rad_w = kwd / 2; //중심픽셀을 기준으로 위아래 양옆으로 얼마나 떨어져있는지
	int rad_h = khg / 2;
	uchar* src_data = (uchar*)src_img.data;
	uchar* dst_data = (uchar*)dst_img.data;
	float* table = new float[kwd * khg](); //커널케이블 동적할당
	float tmp;

	//픽셀 인덱싱(가장자리 제외)
	for (int c = rad_w + 1; c < wd - rad_w; c++) { //가로축. col idx
		for (int r = rad_h + 1; r < hg - rad_h; r++) { //세로축. row idx
			
		 //커널 테이블 초기화
			int index = 0;
			for (int i = -rad_w; i <= rad_w; i++) {
				for (int j = -rad_h; j <= rad_h; j++) {
					tmp = src_data[(r + j) * wd + c + i]; //중심픽셀기준으로 커널테이블위한 인덱스 계산
					table[index++] = tmp; //인덱스에 해당하는 픽셀값 저장
				}
			}

			// 중간 값 찾기
			std::sort(table, table + kwd * khg); //오름차순 정렬
			uchar median = table[kwd * khg / 2]; //중간값 구함

			// Salt and Pepper Noise 제거
			if (median == 0 || median == 255) {//중간값이 0혹은 255인 경우
				dst_data[r * wd + c] = src_data[r * wd + c]; // 원본 이미지 값으로 대체
			}
			else {
				dst_data[r * wd + c] = median; //중간값으로 대체
			}
		}
	}
	delete table; //동적할당 해제
}
void doMedianEx() {
	cout << "--- doMedianEx() --- \n" << endl;
	//입력
	Mat src_img = imread("C:\\DIP_Pic\\6\\salt_pepper.png", 0);
	if (!src_img.data) printf("No image data \n");
	//median 필터링수행
	Mat dst_img;
	myMedian(src_img, dst_img, Size(3, 3));
	//출력
	Mat result_img;
	hconcat(src_img, dst_img, result_img);
	imshow("doMedianEx()", result_img);

}

//5x5필터링비교를 위해 작성하였음
void doMedianEx2() {
	cout << "--- doMedianEx() --- \n" << endl;
	//입력
	Mat src_img = imread("C:\\DIP_Pic\\6\\salt_pepper.png", 0);
	if (!src_img.data) printf("No image data \n");
	//median 필터링수행
	Mat dst_img;
	myMedian(src_img, dst_img, Size(5, 5));
	//출력
	Mat result_img;
	hconcat(src_img, dst_img, result_img);
	imshow("doMedianEx2", result_img);
	waitKey(0);
}

int main() {
	//Mat src_img = imread("C:\\DIP_Pic\\6\\salt_pepper.png", 0); // 이미지 읽기
	// int flags = IMREAD_COLOR 또는 int flags = 1 -> 컬러 영상으로 읽음
	// int flags = IMREAD_GRAYSCALE 또는 int flags = 0 -> 흑백 영상으로 읽음
	// int flags = IMREAD_UNCHANGED 또는 int flags = -1 -> 원본 영상의 형식대로 읽음


	doMedianEx(); //median 필터 실행
	doMedianEx2(); //median 필터 실행

	waitKey(0); // 키 입력 대기(0: 키가 입력될 때 까지 프로그램 멈춤)
	destroyWindow("Test window"); // 이미지 출력창 종료
	//imwrite("gear.jpg", doL); // 이미지 쓰기

	return 0;
}