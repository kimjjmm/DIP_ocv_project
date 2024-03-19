#include <opencv2/opencv.hpp>
#include <iostream>
#include "opencv2/core/core.hpp" // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더
#include <iostream>
#include <cmath>

using namespace cv;
using namespace std;


struct Point { //point 구조체 정의
    int x;
    int y;
    Point(int x, int y) : x(x), y(y) {} //구조체 생성자
};
struct Size { //size 구조체 정의
    int width;
    int height;
    Size(int width, int height) : width(width), height(height) {} //구조체 생성자
};

//이미지 회전
void getMyRotation(const Mat& src, Mat& dst, double angle) {
    cv::Point center(src.cols / 2, src.rows / 2); //이미지 중심좌표 구하기
    double radian = angle * CV_PI / 180.0; //라디안 값으로 변환
    double cos_val = cos(radian); //cos 값 계산
    double sin_val = sin(radian); //sin 값 계산

    //회전 변환 행렬
    double rotation_matrix_data[] = { cos_val, -sin_val, (1 - cos_val) * center.x + sin_val * center.y,
                                      sin_val, cos_val, -sin_val * center.x + (1 - cos_val) * center.y };

    Mat rotation_matrix(2, 3, CV_64F, rotation_matrix_data);

    //회전 변환 적용
    warpAffine(src, dst, rotation_matrix, src.size());

}


int main() {

    Mat src = imread("C:\\DIP_Pic\\12\\Lenna.png", 1); //이미지 읽기
    Mat dst;

    double angle = -45.0; //회전할 각도(반시계방향으로 45도). 시계방향으로 회전하고 싶다면 45를 입력하면 됨
    getMyRotation(src, dst, angle);//이미지 회전

    //이미지 저장 
    imwrite("nonrot.jpg", src); //회전 전
    imwrite("rot.jpg", dst); //회전 후

    //이미지 출력
    imshow("nonrot", src); //회전 전
    imshow("rot", dst); //회전 후
    waitKey(0);

    destroyAllWindows();

    return 0;
}
