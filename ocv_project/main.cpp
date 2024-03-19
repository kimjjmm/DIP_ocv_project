
//HW img1

#include <iostream>
#include<cstdlib>
#include<time.h>
#include "opencv2/core/core.hpp" // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더

using namespace cv;
using namespace std;


void SpreadSalts(Mat img, int num) {
	srand((int)time(NULL)); //실행시킬 때마다 랜덤값을 가지기 위해 램덤함수를 작성
	
	int sumRed = 0;
	int sumGreen = 0;
	int sumBlue = 0;

	for (int n = 0; n < num; n++) { //입력한 점의 갯수만큼 반복문을 실행
		int x = rand() % img.cols; //img.cols는 이미지의 폭 정보를 저장
		int y = rand() % img.rows; //img.rows는 이미지의 높이 정보를 저장
		int i = rand() % 3; // r,g,b를 랜덤으로 생성하기 위해 3의 나머지를 얻음

		 if(img.channels() == 3) {
			img.at<Vec3b>(y, x)[i] = 255; 
			//i가 0일 경우 Blue 채널 접근
			//i가 1일 경우 Green 채널 접근
			//i가 2일 경우 Red 채널 접근 
		}
		
		 if (i == 0) { //Blue 일 때 1씩 증가
			 sumBlue++;
		 }
		 else if (i == 1) {//Green일 때 1씩 증가
			 sumGreen++;
		 }
		 else if (i == 2) {//Red일 때 1씩 증가
			 sumRed++;
		 }
	}
	//카운트 한 값 출력
	cout << "B : " << sumBlue << endl; 
	cout << "G : " << sumGreen << endl;
	cout << "R : " << sumRed << endl;

}


int main() {
	Mat src_img = imread("C:\\DIP_Pic\\img1.jpg",1); // 이미지 읽기
	// int flags = IMREAD_COLOR 또는 int flags = 1 -> 컬러 영상으로 읽음
	// int flags = IMREAD_GRAYSCALE 또는 int flags = 0 -> 흑백 영상으로 읽음
	// int flags = IMREAD_UNCHANGED 또는 int flags = -1 -> 원본 영상의 형식대로 읽음
	SpreadSalts(src_img, 100000);
	imshow("Test window", src_img); // 이미지 출력
	waitKey(0); // 키 입력 대기(0: 키가 입력될 때 까지 프로그램 멈춤)
	destroyWindow("Test window"); // 이미지 출력창 종료
	imwrite("img1.jpg", src_img); // 이미지 쓰기
	return 0;
}



// HW Img2

#include <iostream>
#include "opencv2/core/core.hpp" // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더

using namespace cv;
using namespace std;

Mat darker(Mat img) {
	Mat img2;
	for (int m = 0; m < img.cols; m++) {
		for (int n = 0; n < img.rows; n++) {
			img2 = img.at<uchar>(m,n) + m;
			return img2;
		}
	}
}


int main() {
	Mat src_img = imread("C:\\DIP_Pic\\img2.jpg", 0); // 이미지 읽기
	// int flags = IMREAD_COLOR 또는 int flags = 1 -> 컬러 영상으로 읽음
	// int flags = IMREAD_GRAYSCALE 또는 int flags = 0 -> 흑백 영상으로 읽음
	// int flags = IMREAD_UNCHANGED 또는 int flags = -1 -> 원본 영상의 형식대로 읽음
	//Mat black(src_img.size(), CV_8UC3, Scalar(0));
	darker(src_img);
	imshow("Test window", src_img); // 이미지 출력
	waitKey(0); // 키 입력 대기(0: 키가 입력될 때 까지 프로그램 멈춤)
	destroyWindow("Test window"); // 이미지 출력창 종료
	imwrite("img2.jpg", src_img); // 이미지 쓰기
	return 0;
}

