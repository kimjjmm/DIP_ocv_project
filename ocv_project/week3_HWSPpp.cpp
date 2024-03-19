//week3_HWSPppp.cpp
//3번: salt and pepper noise주고 gaussian filter 적용
#include <iostream>
#include<cstdlib>
#include<time.h>
#include "opencv2/core/core.hpp" // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더

using namespace cv;
using namespace std;

//salt and pepper noise 뿌리는 함수
void SpreadSaltsAndPepper(Mat img, int num) {
    srand((unsigned int)time(NULL));//실행시킬 때마다 랜덤값을 가지기 위해 랜덤함수를 작성
    for (int n = 0; n < num; n++) {

        int x = rand() % img.cols;
        int y = rand() % img.rows;
        int i = rand() % 2; //0,1

        if (img.channels() == 1) { //채널이 1개(흑백)인 경우
            if (i == 0) { //i값이 0일경우 흰색점 뿌리기
                img.at<uchar>(y, x) = 255;
            }
            else //그 외의 경우(1일경우) 검은점 뿌리기
            {
                img.at<uchar>(y, x) = 0;
            }
        }
    }

}
int myKernelConv9x9(uchar* arr, int kernel[][9], int x, int y, int width, int height) {
    int sum = 0;
    int sumKernel = 0;

    //특정 화소의 모든 이웃화소에 대해 계산하도록 반복문 구성
    for (int j = -4; j <= 4; j++) {//9x9로 하기위해선 범위로 -4~4로 해주어야한다.
        for (int i = -4; i <= 4; i++) {
            if ((y + j) >= 0 && (y + j) < height && (x + i) >= 0 && (x + i) < width) {
                //영상 가장자리에서 영상 밖의 화소를 읽지 않도록 하는 조건문
                sum += arr[(y + j) * width + (x + i)] * kernel[i + 4][j + 4];
                sumKernel += kernel[i + 4][j + 4];
            }
        }
    }
    if (sumKernel != 0) { return sum / sumKernel; } //합이 1로 정규화되도록 해 영상의 밝기변화 방지
    else { return sum; }
}
//가우시안필터 함수
Mat mygaussianFilter(Mat srcImg) {
    int width = srcImg.cols; //입력받은 이미지의 행과 열로 높이와 폭을 지정
    int height = srcImg.rows;
    int kernel[9][9] //등방형으로 원소를 넣어준다
        = { 1,4,7,10,7,4,1,
        4,16,26,34,26,16,4,
        7,26,41,52,41,26,7,
        10,34,51,64,52,34,10,
        16,40,59,70,59,40,13,
        10,34,51,64,52,34,10,
        7,26,41,52,41,26,7,
        4,16,26,34,26,16,4,
        1,4,7,10,7,4,1
    };

    Mat dstImg(srcImg.size(), CV_8UC1); //채널 1개로 흑백
    uchar* srcData = srcImg.data;
    uchar* dstData = dstImg.data;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            dstData[y * width + x] = myKernelConv9x9(srcData, kernel, x, y, width, height);
            //앞서 구현한 convolution에 마스크 배열을 입력해 사용
        }
    }
    return dstImg;
}

int main() {
	Mat src_img = imread("C:\\DIP_Pic\\gear.jpg",0); // 이미지 읽기
	// int flags = IMREAD_COLOR 또는 int flags = 1 -> 컬러 영상으로 읽음
	// int flags = IMREAD_GRAYSCALE 또는 int flags = 0 -> 흑백 영상으로 읽음
	// int flags = IMREAD_UNCHANGED 또는 int flags = -1 -> 원본 영상의 형식대로 읽음
    SpreadSaltsAndPepper(src_img, 10000); //원본이미지에 salt and pepper noise가 뿌려짐
    Mat SPGau = mygaussianFilter(src_img);//noise가 뿌려진 이미지에 가우시안필터 적용
	imshow("Salt and Pepper wiht Gau", SPGau); // 이미지 출력
	waitKey(0); // 키 입력 대기(0: 키가 입력될 때 까지 프로그램 멈춤)
	destroyWindow("Test window"); // 이미지 출력창 종료
	imwrite("img1.jpg", src_img); // 이미지 쓰기
	return 0;
}
