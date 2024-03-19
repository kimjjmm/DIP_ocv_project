//week3_HWGP.cpp
//6번:컬러영상에 대한 Laplacian pyramid 구축하고 결과 확인
#include <opencv2/opencv.hpp>
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
//laplacian pyramid구현
vector<Mat> myLaplacianPyramid(Mat srcImg) {
    vector<Mat> Vec;
    for (int i = 0; i < 4; i++) {
        if (i != 3) {
            Mat highImg = srcImg; //수행하기 이전 영상을 백업
            srcImg = mySampling(srcImg);
            srcImg = mygaussianFilter(srcImg);
            Mat lowImg = srcImg;
            resize(lowImg, lowImg, highImg.size()); //작아진 영상을 백업한 영상의 크기로 확대
            Vec.push_back(highImg - lowImg + 128);
            //컨테이너에 삽입. 128을 더한 것은 오버플로우를 방지하기 위함
        }
        else {
            Vec.push_back(srcImg);
        }
    }
    return Vec;
}
int main() {
    Mat src_img = imread("C:\\DIP_Pic\\gear.jpg", 1); // 이미지 읽기
    vector<Mat> VecLap = myLaplacianPyramid(src_img); //laplacian pyramid확보
    // int flags = IMREAD_COLOR 또는 int flags = 1 -> 컬러 영상으로 읽음
    // int flags = IMREAD_GRAYSCALE 또는 int flags = 0 -> 흑백 영상으로 읽음
    // int flags = IMREAD_UNCHANGED 또는 int flags = -1 -> 원본 영상의 형식대로 읽음
    imshow("VecLap0", VecLap[0]);
    imshow("VecLap1", VecLap[1]);
    imshow("VecLap2", VecLap[2]);
    imshow("VecLap3", VecLap[3]);
    waitKey(0);
    destroyWindow("VecLap3");

    reverse(VecLap.begin(), VecLap.end());
    //작은 영상부터 처리하기 위해 vector순서를 반대로 

    Mat dst_img;
    for (int i = 0; i < VecLap.size(); i++) {
        //vector의 크기만큼 반복
        if (i == 0) {
            dst_img = VecLap[i];
           
        }
        else {
            resize(dst_img, dst_img, VecLap[i].size());
            dst_img = dst_img + VecLap[i] - 128;
            //영상ㅇ르 다시 더해 큰 영상으로 복원
            //오버플로우 방지용으로 더했던 128을 다시 빼줌
        }
        string fname = "ex5_lap_pyr" + to_string(i) + ".png";
        imwrite(fname, dst_img);
        imshow("EX5", dst_img);

        waitKey(0);
        destroyWindow("EX5");
    }
    return 0;
}