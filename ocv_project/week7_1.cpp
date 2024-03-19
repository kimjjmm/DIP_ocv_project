#include <opencv2/opencv.hpp>
#include <iostream>
#include "opencv2/core/core.hpp" // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더
using namespace cv;
using namespace std;

Mat MyBgr2Hsv(Mat src_img) { 
    //BGR색 공간으로 표현된 이미지를 HSV로 변환
    double b, g, r, h, s, v;
    Mat dst_img(src_img.size(), src_img.type());
    //입력이미지와 같은 크기와 타입을 가지는 출력이미지 생성
    for (int y = 0; y < src_img.rows; y++) {
        for (int x = 0; x < src_img.cols; x++) {
            b = (double)src_img.at<Vec3b>(y, x)[0];
            g = (double)src_img.at<Vec3b>(y, x)[1];
            r = (double)src_img.at<Vec3b>(y, x)[2];
            //모든 픽셀에 대해 반복하고 픽셀의 BGR값을 b,g,r변수에 저장

         
            // BGR 값을 HSV 값으로 변환
            double max_val = max(max(b, g), r);
            double min_val = min(min(b, g), r);
            double delta = max_val - min_val; 
            //delta: 최대값 - 최소값 차이

            //Hue 계산
            if (max_val == r) {
                h = 60 * (0 + (g - b) / (max_val - min_val));
            }
            if (max_val == g) {
                h = 60 * (2 + (b - r) / (max_val - min_val));
            }
            if (max_val == b) {
                h = 60 * (4 + (r - g) / (max_val - min_val));
            }
            if (h < 0) {
                h = h + 360; // hue가 음수이면, h +30으로 hue를 초기화
            }

            //Saturation 계산
            if (delta == 0) { //max-min = 0
                s = 0;
            }
            else
                s = (max_val - min_val) / max_val;

            //Value계산
            v = max_val; //value값은 밝기를 나타내는 값으로서, 픽셀 중 가장 큰 값은 max_val이 value가 됨
            
           // Mat hsv_img;
            dst_img.at<Vec3b>(y, x)[0] = (uchar)(h/2);
            dst_img.at<Vec3b>(y, x)[1] = (uchar)(s*255);
            dst_img.at<Vec3b>(y, x)[2] = (uchar)v;
        }
    }

    return dst_img;
}



Mat MyinRange(Mat src_img, Scalar lower, Scalar upper) {
    
    Mat dst_img(src_img.rows, src_img.cols, CV_8UC1);
    for (int y = 0; y < src_img.rows; y++) {
        for (int x = 0; x < src_img.cols; x++) {
            double b = (double)src_img.at<Vec3b>(y, x)[0];
            double g = (double)src_img.at<Vec3b>(y, x)[1];
            double r = (double)src_img.at<Vec3b>(y, x)[2];
            //모든 픽셀에 대해 반복하고 값을 bgr에 각각 저장

            //bgr의 순서로
            bool in_range = (lower[0] <= b && b <= upper[0]) &&
                (lower[1] <= g && g <= upper[1]) &&
                (lower[2] <= r && r <= upper[2]);
            if (in_range) {//lower-upper 범위 내에 있는 픽셀은 흰색
                dst_img.at<uchar>(y, x) = 255;
            }
            else {//범위 외에 있는 픽셀은 검은색
                dst_img.at<uchar>(y, x) = 0;
            }

        }
    }
    return dst_img;
}


int main() {
    //빨강
    Scalar lower_red = Scalar(0, 50, 100);
    Scalar upper_red = Scalar(10, 255, 255);
    //파랑
    Scalar lower_blue = Scalar(100, 50, 100);
    Scalar upper_blue = Scalar(120, 255, 255);
    //초록
    Scalar lower_green = Scalar(30, 30, 50);
    Scalar upper_green = Scalar(50, 255, 255);



    Mat src_img = imread("C:\\DIP_Pic\\7\\apple.jpg", 1); // 이미지 읽기
     //hsv바꾸기
    Mat hsv_img = MyBgr2Hsv(src_img);

    //색별로 마스크 만들기
    Mat mask_red = MyinRange(hsv_img, lower_red, upper_red);
    Mat mask_blue = MyinRange(hsv_img, lower_blue, upper_blue);
    Mat mask_green = MyinRange(hsv_img, lower_green, upper_green);

    // 각 영역에서 가장 많은 픽셀을 가진 색상 출력
    int red_pixels = countNonZero(mask_red);
    int blue_pixels = countNonZero(mask_blue);
    int green_pixels = countNonZero(mask_green);

    Mat final;

    //이미지에서 가장 많은 픽셀값을 가진 색상 찾기
    if (red_pixels > blue_pixels && red_pixels > green_pixels)
    {
        cout << "The image is red" << endl;
        bitwise_and(src_img, src_img, final, mask_red);
    }
    else if (blue_pixels > red_pixels && blue_pixels > green_pixels)
    {
        cout << "The image is blue" << endl;
        bitwise_and(src_img, src_img, final, mask_blue);
    }
    else if (green_pixels > red_pixels && green_pixels > blue_pixels)
    {
        cout << "The image is green" << endl;
        bitwise_and(src_img, src_img, final, mask_green);
    }
    else
    {
        cout << "The image contains multiple colors" << endl;
    }

    imshow("final", final);

    waitKey(0); // 키 입력 대기(0: 키가 입력될 때 까지 프로그램 멈춤)
    
    return 0;
}
