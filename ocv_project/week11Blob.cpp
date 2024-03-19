#include <opencv2/opencv.hpp>
#include <iostream>
#include "opencv2/core/core.hpp" // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더

using namespace cv;
using namespace std;

void cvBlobDetection() {
	Mat img = imread("C:\\DIP_Pic\\11\\coin.png", IMREAD_COLOR); //이미지 읽기
	SimpleBlobDetector::Params params; //파라미터 설정
	params.minThreshold = 10; //최소 임계값 정하는 파라미터
	params.maxThreshold = 300; //최대 임계값 정하는 파라미터
	params.filterByArea = true; //면적에 대한 필터링 여부
	params.minArea = 10; //최소면적 파라미터
	params.maxArea = 7000; //최대면적 파라미터
	params.filterByCircularity = true; //물체의 원형치 필터링 여부 결정하는 파라미터
	params.minCircularity = 0.5; //최소 원형치 설정 파라미터
	params.filterByConvexity = true; //물체의 오목함에 대한 필터링 여부 결정하는 파라미터
	params.minConvexity = 0.9; //최소 오목함 파라미터
	params.filterByInertia = true; //타원이 원에 얼마나 가까운지 제안
	params.minInertiaRatio = 0.9; //최소 타원/원 비율 설정

	Ptr<SimpleBlobDetector>detector = SimpleBlobDetector::create(params); //create가 parameter을 전달

	//keypoint에 블랍을 적용한 결과가 저장
	std::vector<KeyPoint> keypoints; 
	detector->detect(img, keypoints); 

	Mat result; 
	//이미지에 keypoints 그리기
	drawKeypoints(img, keypoints, result, Scalar(255, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

	cout << "동전의 개수: " << keypoints.size() << endl; //동전 개수 출력

	imshow("keypoints", result); //이미지 출력
	waitKey(0);
	destroyAllWindows();
}

int main() {
	cvBlobDetection();
}