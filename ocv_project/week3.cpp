#include <iostream>
#include "opencv2/core/core.hpp" // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더

using namespace cv;
using namespace std;

Mat myCopy(Mat srcImg) {
	int width = srcImg.cols;
	int height = srcImg.rows;
	Mat dstImg(srcImg.size(), CV_8UC1);
	uchar* srcData = srcImg.data;
	uchar* dstData = dstImg.data;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			dstData[y * width + x] = srcData[y * width + x];
		}
	}
	return dstImg;
}

int myKernelConv3x3(uchar* arr, int kernel[][3], int x, int y, int width, int height) {
	int sum = 0;
	int sumKernel = 0;
	for (int j = -1; j <= 1; j++) {
		for( int i = -1; i <= 1; i++){
		if ((y + j) >= 0 && (y + j) < height && (x + i) >= 0 && (x + i) < width) {
			sum += arr[(y + j) * width + (x + i)] * kernel[i + 1][j + 1];
			sumKernel += kernel[i + 1][j + 1];
		}
	}
	}
	if (sumKernel != 0) { return sum / sumKernel; }
	else { return sum; }
}
Mat mygaussianFilter(Mat srcImg) {
	int width = srcImg.cols;
	int height = srcImg.rows;
	int kerner[3][3]
		= { 1,2,1,
			2,4,2,
			1,2,1 };
	Mat dstImg(srcImg.size(), CV_8UC1);
	uchar* srcData = srcImg.data;
	uchar* dstData = dstImg.data;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			dstData[y * width + x] = myKernelConv3x3(srcData, kerner, x, y, width, height);
		}
	}
	return dstImg;
}
Mat mysobelFilter(Mat srcImg) {

	int kernelX[3][3] = { 0, 1 ,2,
					 -1, 0, 1,
					 -2, -1, 0 };//대각엣지 45도
	int kernelY[3][3] = { -2,-1,1,
					  -1, 0, 1,
					  0, 1, 2 }; //대각엣지 135도


	Mat dstImg(srcImg.size(), CV_8UC1);
	uchar* srcData = srcImg.data;
	uchar* dstData = dstImg.data;
	int width = srcImg.cols;
	int height = srcImg.rows;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			dstData[y * width + x] = (abs(myKernelConv3x3(srcData, kernelX, x, y, width, height)) + 
									  abs(myKernelConv3x3(srcData, kernelY, x, y, width, height))) / 2;
		}
	}
	return dstImg;
}
Mat mySampling(Mat srcImg) {
	int width = srcImg.cols / 2;
	int height = srcImg.rows / 2;
	Mat dstImg(height, width, CV_8UC1);
	uchar* srcData = srcImg.data;
	uchar* dstData = dstImg.data;

	for (int y = 0; y < height; y++ ) {
		for (int x = 0; y < height; x++) {
			dstData[y * width + x] = srcData[(y * 2) * (width * 2) + (x * 2)];
		}
	}
	return dstImg;
}

vector<Mat> myGaussainPyramid(Mat srcImg) {
	vector<Mat> Vec;

	Vec.push_back(srcImg);
	for (int i = 0; i < 4; i++) {
#if USE_OPENCV
		pyrDown(srcImg, srcImg, Size(srcImg.cols / 2, srcImg.rows / 2));
#else 
		srcImg=mySampling(srcImg);
		srcImg = mygaussianFilter(srcImg);
#endif
		Vec.push_back(srcImg);
	}
	return Vec;
}

vector<Mat> myLapacialPyramid(Mat srcImg) {
	vector<Mat> Vec;
	for (int i = 0; i < 4; i++) {
		if (i != 3) {
			Mat highImg = srcImg;
#if USE_OPENCV
			pyrDown(srcImg, srcImg, Size(srcImg.cols / 2, srcImg.rows / 2));
#else
			srcImg = mySampling(srcImg);
			srcImg = mygaussianFilter(srcImg);
#endif
			Mat lowImg = srcImg;
			resize(lowImg, lowImg, highImg.size());
			Vec.push_back(highImg - lowImg + 128);
		}
		else {
			Vec.push_back(srcImg);
		}
	}
	return Vec;
}





int main() {
	Mat src_img = imread("C:\\DIP_Pic\\gear.jpg", 0); // 이미지 읽기
	// int flags = IMREAD_COLOR 또는 int flags = 1 -> 컬러 영상으로 읽음
	// int flags = IMREAD_GRAYSCALE 또는 int flags = 0 -> 흑백 영상으로 읽음
	// int flags = IMREAD_UNCHANGED 또는 int flags = -1 -> 원본 영상의 형식대로 읽음
	//Mat dsy = myCopy(src_img);
	//Mat ss = mygaussianFilter(src_img);
	//Mat dd= mysobelFilter(src_img);
	//imshow("Test window", src_img); // 이미지 출력
	//imshow("Test windw", dsy); // 이미지 출력

	Mat sobel = mysobelFilter(src_img); //sobel 필터
	imshow("Test indow", sobel); // 이미지 출력	

	//vector<Mat> GP = myGaussainPyramid(src_img);
	//imshow("0", GP[0]);
	//imshow("1", GP[1]);
	//imshow("2", GP[2]);
	//imshow("3", GP[3]);


	//vector<Mat> dd = 

	waitKey(0); // 키 입력 대기(0: 키가 입력될 때 까지 프로그램 멈춤)
	//destroyWindow("Test window"); // 이미지 출력창 종료
	//imwrite("gear.jpg", src_img); // 이미지 쓰기

	return 0;
}
