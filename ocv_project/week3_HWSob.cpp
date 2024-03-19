//week3_HWSob.cpp
//4번:45도와 135도의 대각에지 검출하는 sobel filter구현
#include <iostream>
#include "opencv2/core/core.hpp" // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더
using namespace cv;
using namespace std;


//특정 화소의 모든 이웃화소에 대해 계산하도록 반복문 구성
int myKernelConv3x3(uchar* arr, int kernel[][3], int x, int y, int width, int height) {
    int sum = 0;
    int sumKernel = 0;
    for (int j = -1; j <= 1; j++) { //3x3로 하기위해선 범위로 -1~1로 해주어야한다.
        for (int i = -1; i <= 1; i++) {
            //영상 가장자리에서 영상 밖의 화소를 읽지 않도록 하는 조건문
            if ((y + j) >= 0 && (y + j) < height && (x + i) >= 0 && (x + i) < width) {
                sum += arr[(y + j) * width + (x + i)] * kernel[i + 1][j + 1];
                sumKernel += kernel[i + 1][j + 1];
            }
        }
    }
    if (sumKernel != 0) { return sum / sumKernel; } //합이 1로 정규화되도록 해 영상의 밝기변화 방지
    else { return sum; }
}

Mat mysobelFilter(Mat srcImg) {
    
    //마스크를 직접 지정
    int kernelX[3][3] = { 0, 1 ,2, // 45도 대각에지
                         -1, 0, 1,
                         -2, -1, 0 };
    int kernelY[3][3] = { -2,-1,1, //135도 대각에지
                         -1, 0, 1,
                          0, 1, 2 };

    Mat dstImg(srcImg.size(), CV_8UC1); //채널1개 흑백
    uchar* srcData = srcImg.data;
    uchar* dstData = dstImg.data;
    int width = srcImg.cols;
    int height = srcImg.rows;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            dstData[y * width + x] = (abs(myKernelConv3x3(srcData, kernelX, x, y, width, height)) +
                abs(myKernelConv3x3(srcData, kernelY, x, y, width, height))) / 2;
            //두 에지 결과의 절대값 합 형태로 최종결과 도출
        }
    }
    return dstImg;
}


int main() {
    Mat src_img = imread("C:\\DIP_Pic\\gear.jpg", 0); // 이미지 읽기
    Mat sobel = mysobelFilter(src_img);
    // int flags = IMREAD_COLOR 또는 int flags = 1 -> 컬러 영상으로 읽음
    // int flags = IMREAD_GRAYSCALE 또는 int flags = 0 -> 흑백 영상으로 읽음
    // int flags = IMREAD_UNCHANGED 또는 int flags = -1 -> 원본 영상의 형식대로 읽음

    imshow("soble", sobel);
    waitKey(0); // 키 입력 대기(0: 키가 입력될 때 까지 프로그램 멈춤)
    destroyWindow("Test window"); // 이미지 출력창 종료
    imwrite("langding_gray.png", src_img); // 이미지 쓰기
    return 0;
}