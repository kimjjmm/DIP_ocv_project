//sobel frequency domain 12205102김지민

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
Mat doDft(Mat srcImg) {//영상신호를 주파수 영역으로 바꾸는 함수
    Mat floatImg;
    srcImg.convertTo(floatImg, CV_32F); //32bit floatning point number로 convert
    Mat complexImg;
    dft(floatImg, complexImg, DFT_COMPLEX_OUTPUT); //DFT_COMPLEX_OUTPUT로 수행결과가 복소수가 됨

    return complexImg;
}
Mat getMagnitude(Mat complexImg) { //magnitude 영상 취득
    Mat planes[2];
    split(complexImg, planes);// 실수부, 허수부 분리

    Mat magImg;
    magnitude(planes[0], planes[1], magImg);
    magImg += Scalar::all(1);
    log(magImg, magImg); //log scale 변환 
    //magnitude 취득

    return magImg;
}
Mat getPhase(Mat complexImg) { //phase 영상 취득
    Mat planes[2];
    split(complexImg, planes); //실수부, 허수부 분리
    Mat phaImg;
    phase(planes[0], planes[1], phaImg);
    return phaImg;
}
Mat myNormalize(Mat src) {//정규화
    Mat dst;
    src.copyTo(dst);
    normalize(dst, dst, 0, 255, NORM_MINMAX);
    //변환최소값 0부터 변환최대값 255까지로 변환. NORM_MINMAX는 변환 방식.
    dst.convertTo(dst, CV_8UC1); //0~255. 흑백이미지로

    return dst;
}
Mat centralize(Mat complex) {//좌표계 중앙 이동

    Mat planes[2];
    split(complex, planes);
    int cx = planes[0].cols / 2;
    int cy = planes[1].rows / 2;

    Mat q0Re(planes[0], Rect(0, 0, cx, cy));
    Mat q1Re(planes[0], Rect(cx, 0, cx, cy));
    Mat q2Re(planes[0], Rect(0, cy, cx, cy));
    Mat q3Re(planes[0], Rect(cx, cy, cx, cy));

    Mat tmp; //임시저장
    q0Re.copyTo(tmp);
    q3Re.copyTo(q0Re);
    tmp.copyTo(q3Re);
    q1Re.copyTo(tmp);
    q2Re.copyTo(q1Re);
    tmp.copyTo(q2Re);

    Mat q0Im(planes[1], Rect(0, 0, cx, cy));
    Mat q1Im(planes[1], Rect(cx, 0, cx, cy));
    Mat q2Im(planes[1], Rect(0, cy, cx, cy));
    Mat q3Im(planes[1], Rect(cx, cy, cx, cy));

    q0Im.copyTo(tmp);
    q3Im.copyTo(q0Im);
    tmp.copyTo(q3Im);
    q1Im.copyTo(tmp);
    q2Im.copyTo(q1Im);
    tmp.copyTo(q2Im);

    Mat centerComplex;
    merge(planes, 2, centerComplex);
    return centerComplex;
}
Mat padding(Mat img) {
    int dftRows = getOptimalDFTSize(img.rows);
    int dftCols = getOptimalDFTSize(img.cols);

    Mat padded;
    copyMakeBorder(img, padded, 0, dftRows - img.rows, 0, dftCols - img.cols, BORDER_CONSTANT, Scalar::all(0));
    return padded;
}

int main() {
    Mat src_img = imread("C:\\DIP_Pic\\5\\img2.jpg", 0); // 이미지 읽기
    // int flags = IMREAD_COLOR 또는 int flags = 1 -> 컬러 영상으로 읽음
    // int flags = IMREAD_GRAYSCALE 또는 int flags = 0 -> 흑백 영상으로 읽음
    // int flags = IMREAD_UNCHANGED 또는 int flags = -1 -> 원본 영상의 형식대로 읽음

    Mat sobel = mysobelFilter(src_img);//원본이미지에 sobel필터 적용
    imshow("sobelimg", sobel); //sobel적용 이미지 출력
    imshow("origimg", src_img);

    //dft_origin
    Mat padd_orig = padding(src_img);

    Mat freq_orig = doDft(padd_orig);
    Mat cent_orig = centralize(freq_orig);

    Mat mag_orig = getMagnitude(cent_orig); //magnitude이미지
    Mat origmag = myNormalize(mag_orig);

    Mat pha_orig = getPhase(cent_orig); //phase이미지
    Mat origpha = myNormalize(pha_orig);

    imshow("origlmag", origmag); //원본영상의 magnitude이미지 출력	
    imshow("origpha", origpha); //원본영상의 phase이미지 출력	

    //dft_sobel
    Mat padd_sobel = padding(sobel);

    Mat freq_sobel = doDft(padd_sobel);
    Mat cent_sobel = centralize(freq_sobel);

    Mat mag_sobel = getMagnitude(cent_sobel); //magnitude이미지
    Mat sobelmag = myNormalize(mag_sobel);

    Mat pha = getPhase(cent_sobel); //phase이미지
    Mat sobelpha = myNormalize(pha);


    imshow("sobelmag", sobelmag); //sobel필터 적용 후 magnitude이미지 출력	
    imshow("sobelpha", sobelpha); //sobel필터 적용 후 phase이미지 출력	


    waitKey(0); // 키 입력 대기(0: 키가 입력될 때 까지 프로그램 멈춤)
    destroyWindow("Test window"); // 이미지 출력창 종료

    return 0;
}