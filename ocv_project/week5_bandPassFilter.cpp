//Band pass filter 12205102김지민

#include <iostream>
#include "opencv2/core/core.hpp" // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더

using namespace cv;
using namespace std;

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


Mat setComplex(Mat magImg, Mat phaImg) {
	exp(magImg, magImg);
	magImg -= Scalar::all(1); //magnitude 계산을 반대로 수행

	Mat planes[2];
	polarToCart(magImg, phaImg, planes[0], planes[1]); //극 좌표계 -> 직교좌표계

	Mat complexImg;
	merge(planes, 2, complexImg);
	//실수부, 허수부 합체
	return complexImg;
}

Mat doIdft(Mat complexImg) {//푸리에 역변환
	Mat idftcvt;
	idft(complexImg, idftcvt);
	//IDFT를 이용한 원본 영상 취득

	Mat planes[2];
	split(idftcvt, planes);

	Mat dstImg;
	magnitude(planes[0], planes[1], dstImg);
	normalize(dstImg, dstImg, 255, 0, NORM_MINMAX);
	dstImg.convertTo(dstImg, CV_8UC1);
	// 일반 영상의 type과 표현범위로 변환

	return dstImg;
}
Mat padding(Mat img) { 
	int dftRows = getOptimalDFTSize(img.rows);
	int dftCols = getOptimalDFTSize(img.cols);

	Mat padded;
	copyMakeBorder(img, padded, 0, dftRows - img.rows, 0, dftCols - img.cols, BORDER_CONSTANT, Scalar::all(0));
	return padded;
}

Mat doBPF(Mat srcImg)
{
	//<DFT>
	Mat padImg = padding(srcImg);
	Mat complexImg = doDft(padImg);
	Mat centerComplexImg = centralize(complexImg);
	Mat magImg = getMagnitude(centerComplexImg);
	Mat phaImg = getPhase(centerComplexImg);

	//<BPF>
	double minVal, maxVal;
	Point minLoc, maxLoc;
	minMaxLoc(magImg, &minVal, &maxVal, &minLoc, &maxLoc);//최대값 최소값을 얻어서 정규화
	normalize(magImg, magImg, 0, 1, NORM_MINMAX);

	Mat maskImg = Mat::zeros(magImg.size(), CV_32F);//zeros로 0형렬을 반환해놓기
	circle(maskImg, Point(maskImg.cols / 2, maskImg.rows / 2), 50, Scalar::all(1), -1, -1, 0); //HPF
	circle(maskImg, Point(maskImg.cols / 2, maskImg.rows / 2), 20, Scalar::all(0), -1, -1, 0); //LPF
	
	Mat magImg2;
	multiply(magImg, maskImg, magImg2);
	//magnitude이미지와 마스크이미지를 convolution하여 magImg2 얻을 수 있음

	imshow("BPF", magImg2); 
	//역변환 전 bandpassfilter가 씌워진 magnitude 이미지 출력

	//<IDFT>
	normalize(magImg2, magImg2, (float)minVal, (float)maxVal, NORM_MINMAX);
	Mat complexImg2 = setComplex(magImg2, phaImg);
	Mat dstImg = doIdft(complexImg2);
	return myNormalize(dstImg);
}

int main() {
	Mat src_img = imread("C:\\DIP_Pic\\5\\img1.jpg", 0); // 이미지 읽기
	// int flags = IMREAD_COLOR 또는 int flags = 1 -> 컬러 영상으로 읽음
	// int flags = IMREAD_GRAYSCALE 또는 int flags = 0 -> 흑백 영상으로 읽음
	// int flags = IMREAD_UNCHANGED 또는 int flags = -1 -> 원본 영상의 형식대로 읽음

	Mat bpf = doBPF(src_img); //이미지에 band pass filter 씌우기
	imshow("Test window", bpf); //이미지 출력

	waitKey(0); // 키 입력 대기(0: 키가 입력될 때 까지 프로그램 멈춤)
	destroyWindow("Test window"); // 이미지 출력창 종료
	//imwrite("gear.jpg", doL); // 이미지 쓰기

	return 0;
}