#include <opencv2/opencv.hpp>
#include <iostream>
#include "opencv2/core/core.hpp" // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더

using namespace cv;
using namespace std;

void cvFeatureSIFT() {
	Mat img = imread("C:\\DIP_Pic\\11\\church.jpg", 1); //이미지 읽기

	Mat gray;
	cvtColor(img, gray, CV_BGR2GRAY); //컬러 이미지를 그레이스케일로 변환

	Ptr<SiftFeatureDetector> detector = SiftFeatureDetector::create(); //sift검출기 생성
	vector<KeyPoint>keypoints;
	detector->detect(gray, keypoints); //sift 키포인트(이미지에서 특징적인 위치) 검출

	Mat result;
    //원본이미지 위에 키포인트를 그림
	drawKeypoints(img, keypoints, result, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

	//imwrite("sift_result.jpg", result);
	imshow("Sift result", result); //이미지 출력
	waitKey(0);
	cvDestroyAllWindows();
}

void warpPersSift() {
    Mat src = imread("C:\\DIP_Pic\\11\\church.jpg", 1); //이미지 읽기
    Mat dst;

    Point2f src_p[4], dst_p[4]; //번환 전, 후 꼭짓점 변수

    //변환 전 꼭짓점
    src_p[0] = Point2f(0, 0);
    src_p[1] = Point2f(512, 0);
    src_p[2] = Point2f(0, 512);
    src_p[3] = Point2f(512, 512);
    //cout << src.size()<<endl;

    //변환 후 꼭짓점
    dst_p[0] = Point2f(0, 0);
    dst_p[1] = Point2f(512, 0);
    dst_p[2] = Point2f(0, 512);
    dst_p[3] = Point2f(412, 412);

    Mat pers_mat = getPerspectiveTransform(src_p, dst_p);//원근변환 행렬 생성
    warpPerspective(src, dst, pers_mat, src.size());//원근변환. 
    imshow("warpResult", dst); //이미지 출력

    //밝기 조절
    Mat hsv;
    cvtColor(dst, hsv, COLOR_BGR2HSV); //이미지를 HSV색공간으로 변환
    vector<Mat> channels;
    split(hsv, channels);//HSV이미지를 채널로 분리
    channels[2] += 100; //밝기채널을 100만큼 밝기 증가
    merge(channels, hsv);//분리된 채널 다시 병합
    cvtColor(hsv, dst, COLOR_HSV2BGR); //HSV를 BGR로 변환

    Mat gray;
    cvtColor(dst, gray, CV_BGR2GRAY); //컬러 이미지를 그레이스케일로 변환

	Ptr<SiftFeatureDetector> detector = SiftFeatureDetector::create();
	vector<KeyPoint>keypoints;
	detector->detect(gray, keypoints); //sift 키포인트(이미지에서 특징적인 위치) 검출

	Mat result;
    //원본이미지 위에 키포인트를 그림

	drawKeypoints(dst, keypoints, result, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    cout << result.size() << endl;

	//imwrite("warpsift_result.jpg", result);
	imshow("Sift result", result); //이미지 출력
	waitKey(0);
	cvDestroyAllWindows();

}

int main() {
    cvFeatureSIFT();
    warpPersSift();
}