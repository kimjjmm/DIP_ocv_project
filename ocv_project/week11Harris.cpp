#include <opencv2/opencv.hpp>
#include <iostream>
#include "opencv2/core/core.hpp" // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더

#include <opencv2/objdetect/objdetect.hpp> //객체 검출에 사용되는 함수 포함 헤더
#include <opencv2/features2d.hpp>//이미지 특징점 검출 및 매칭 함수 포함 헤더

using namespace cv;
using namespace std;

void cvHarrisConer() {
	Mat img = imread("C:\\DIP_Pic\\11\\square.png"); //이미지 불러오기
	//이미지 없는 경우 예외
	if (img.empty()) {
		cout << "Empty imged!\n";
		exit(-1);
	}
	//이미지 크기 조절
	resize(img, img, Size(500, 500), 0, 0, INTER_CUBIC);

	//그레이 스케일 이미지 생성
	Mat gray;
	cvtColor(img, gray, CV_BGR2GRAY);

	//해리스코너 검출 수행
	Mat harr;
	cornerHarris(gray, harr, 2, 3, 0.05, BORDER_DEFAULT);
	
	//결과이미지 normalize
	normalize(harr, harr, 0,255,NORM_MINMAX,CV_32FC1,Mat());
	Mat harr_abs;
	convertScaleAbs(harr, harr_abs);

	//임계값 기준으로 코너에 원 그리기
	int thresh = 125;
	Mat result = img.clone();
	for (int y = 0; y < harr.rows; y += 1) {
		for (int x = 0; x < harr.cols; x += 1) {
			if ((int)harr.at<float>(y, x) > thresh)
				circle(result, Point(x, y), 30, Scalar(0, 0, 0), 0, 4, 0);
		}
	}

	//이미지 출력
	imshow("Souce image", img); //입력이미지
	imshow("Harris image", harr_abs); //해리스코너 시각화 결과
	imshow("Target imag", result); //원검출 결과

	//허프변환을 위한 이미지 복사
	Mat img_hough;
	result.copyTo(img_hough);

	//그레이스케일로 변환
	Mat gray_result;
	cvtColor(img_hough, gray_result, CV_BGR2GRAY);

	//허프 원 검출 수행
	vector<Vec3f>circles;
	HoughCircles(gray_result, circles, HOUGH_GRADIENT, 1, 100, 50, 25, 0, 0);
	for (size_t i = 0; i < circles.size(); i++) {
		Vec3i c = circles[i];
		Point center(c[0], c[1]);
		int radius = c[2];

		//검출된 원 그리기
		circle(gray_result, center, radius, Scalar(0, 255, 0), 2);
		circle(gray_result, center, 2, Scalar(0, 0, 255), 3);
	}
	//원 개수 출력
	cout <<  circles.size()<<"각형" << endl;

	//원검출 이미지 출력
	imshow("img_hough", gray_result);

	waitKey(0); ///윈도우 종료 대기
	destroyAllWindows(); //모든윈도우 종료

}

int main() {
	cvHarrisConer();
	//myHarrisConer();
}