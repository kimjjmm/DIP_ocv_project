//week3_HWGP.cpp
//5번:컬러영상에 대한 Gaussian pyramid 구축하고 결과 확인
#include <iostream>
#include "opencv2/core/core.hpp" // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더
using namespace cv;
using namespace std;

//특정 화소의 모든 이웃화소에 대해 계산하도록 반복문 구성
int myKernelConv3x3(uchar* arr, int kernel[][3], int x, int y, int width, int height, int channel) {
//컬러영상으로 변환해야하기 때문에 매개변수에 channel을 추가하였다 
    int sum = 0;
    int sumKernel = 0;
    for (int j = -1; j <= 1; j++) {//3x3로 하기위해선 범위로 -1~1로 해주어야한다.
        for (int i = -1; i <= 1; i++) {
            //3x3로 하기위해선 범위로 -1~1로 해주어야한다.
            if ((y + j) >= 0 && (y + j) < height && (x + i) >= 0 && (x + i) < width) {
                sum += arr[(y + j) * width * 3 + (x + i) * 3 + channel] * kernel[i + 1][j + 1];
                sumKernel += kernel[i + 1][j + 1];
                //컬러로 변환하기 위해 3씩 곱해 늘려주었다

            }
        }
    }
    if (sumKernel != 0) { return sum / sumKernel; }//합이 1로 정규화되도록 해 영상의 밝기변화 방지
    else { return sum; }
}

Mat mySampling(Mat srcImg) {
    int width = srcImg.cols / 2;
    int height = srcImg.rows / 2;
    Mat dstImg(height, width, CV_8UC3); // 채널을 3개로 바꿔서 컬러 이미지로 변경
    uchar* srcData = srcImg.data;
    uchar* dstData = dstImg.data;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            for (int c = 0; c < 3; c++) { // 3개의 채널을 각각 처리
                dstData[(y * width + x) * 3 + c] = srcData[((y * 2) * (width * 2) + (x * 2)) * 3 + c];
            }
        }
    }
    return dstImg;
}

Mat mygaussianFilter(Mat srcImg) {
    int width = srcImg.cols;
    int height = srcImg.rows;
    int kernel[3][3] = { {1,2,1}, {2,4,2}, {1,2,1} };
    Mat dstImg(srcImg.size(), CV_8UC3); // 채널을 3개로 바꿔서 컬러 이미지로 변경
    uchar* srcData = srcImg.data;
    uchar* dstData = dstImg.data;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            for (int c = 0; c < 3; c++) {
                dstData[(y * width + x) * 3 + c] = myKernelConv3x3(srcData + c, kernel, x, y, width, height, c);
            }
        }
    }
    return dstImg;
}

//down sampling과 gaussian 필터 기반 smoothing을 조합해 점차 작은 해상도의 영상을 반복적으로 생성
vector<Mat> myGaussainPyramid(Mat srcImg) {
    vector<Mat> Vec;//여러 영상을 모아서 저장하기 위해 STL의 vector컨테이너 사용

    Vec.push_back(srcImg); 
    for (int i = 0; i < 4; i++) {
        srcImg = mySampling(srcImg); // 앞서 구현한 down sampling
        srcImg = mygaussianFilter(srcImg); //vector컨테이너에 하나씩 처리결과를 삽입
        Vec.push_back(srcImg);
    }
    return Vec;
}

int main() {
    Mat src_img = imread("C:\\DIP_Pic\\gear.jpg", 1); // 이미지 읽기
    vector<Mat> GauPyr = myGaussainPyramid(src_img);//가우시안 피라미드 적용한 변수
    // int flags = IMREAD_COLOR 또는 int flags = 1 -> 컬러 영상으로 읽음
    // int flags = IMREAD_GRAYSCALE 또는 int flags = 0 -> 흑백 영상으로 읽음
    // int flags = IMREAD_UNCHANGED 또는 int flags = -1 -> 원본 영상의 형식대로 읽음
    
    imshow("GauPyr0", GauPyr[0]); //가우시안 피라미드를 서로 다른 창으로 하여 출력
    imshow("GauPyr1", GauPyr[1]); //큰 창부터 출력된다
    imshow("GauPyr2", GauPyr[2]);
    imshow("GauPyr3", GauPyr[3]);
    imshow("GauPyr4", GauPyr[4]);

    waitKey(0); // 키 입력 대기(0: 키가 입력될 때 까지 프로그램 멈춤)
    destroyWindow("Test window"); // 이미지 출력창 종료
    imwrite("langding_gray.png", src_img); // 이미지 쓰기
    return 0;
}