#include <iostream>
#include "opencv2/core/core.hpp" // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더

using namespace cv;
using namespace std;

void CvColorModels(Mat bgr_img) {
	Mat gray_img, rgb_img, hsv_img, yuv_img, xyz_img;

	cvtColor(bgr_img, gray_img, cv::COLOR_BGR2GRAY);
	cvtColor(bgr_img, rgb_img, cv::COLOR_BGR2RGB);
	cvtColor(bgr_img, hsv_img, cv::COLOR_BGR2HSV);
	cvtColor(bgr_img, yuv_img, cv::COLOR_BGR2YCrCb);
	cvtColor(bgr_img, xyz_img, cv::COLOR_BGR2XYZ);

	Mat print_img;
	bgr_img.copyTo(print_img);
	cvtColor(gray_img, gray_img, cv::COLOR_GRAY2BGR);
	hconcat(print_img, gray_img, print_img);
	hconcat(print_img, rgb_img, print_img);
	hconcat(print_img, hsv_img, print_img);
	hconcat(print_img, yuv_img, print_img);
	hconcat(print_img, xyz_img, print_img);

	imshow("results", print_img);
//	imwrite("C:\\DIP_Pic\\7\\fruits.png", print_img);

	waitKey(0);
}

Mat 
//Mat GetYCbCr(Mat src_img) {
//	double b, g, r, y, cb, cr;
//	Mat dst_img;
//	src_img.copyTo(dst_img);
//
//	for (int row = 0; row < dst_img.rows; row++) {
//		for (int col = 0; col < dst_img.cols; col++) {
//			b = (double)dst_img.at<Vec3b>(row, col)[0];
//			g = (double)dst_img.at<Vec3b>(row, col)[1];
//			r = (double)dst_img.at<Vec3b>(row, col)[2];
//
//			y = 0.2627 * r + 0.678 * g + 0.0593 * b;
//			cb = cb > 255.0 ? 255.0 : cb < 0 ? 0 : cb;
//			cr = cr > 255.0 ? 255.0 : cr < 0 ? 0 : cr;
//
//			dst_img.at<Vec3b>(row, col)[0] = (uchar)y;
//			dst_img.at<Vec3b>(row, col)[1] = (uchar)cb;
//			dst_img.at<Vec3b>(row, col)[2] = (uchar)cr;
//
//		}
//	}
//	return dst_img;
//}
//Mat CvKMeans(Mat src_img, int k) {
//	//<2차원 영상 -> 1차원 벡터>
//	Mat samples(src_img.rows * src_img.cols, src_img.channels(), CV_32F);
//	for (int y = 0; y < src_img.rows; y++) {
//		for (int x = 0; x < src_img.cols; x++) {
//			if (src_img.channels() == 3) {
//				for (int z = 0; z < src_img.channels(); z++) {
//					samples.at<float>(y + x * src_img.rows, z) =
//						(float)src_img.at<Vec3b>(y, x)[z];
//				}
//			}
//			else {
//				samples.at<float>(y + x * src_img.rows) =
//					(float)src_img.at<uchar>(y, x);
//			}
//		}
//	}
//	//<OpenCV k-meas 수행>
//	Mat labels; //군집판별 결과가 담길 1차원 벡터
//	Mat centers;
//	int attempts = 5;
//	kmeans(samples, k, labels,
//		TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10000, 0.0001),
//		attempts, KMEANS_PP_CENTERS,
//		centers);
//
//	Mat dst_img(src_img.size(), src_img.type());
//	for (int y = 0; y < src_img.rows; y++) {
//		for (int x = 0; x < src_img.cols; x++) {
//			int cluster_idx = labels.at<int>(y + x * src_img.rows, 0);
//			if (src_img.channels() == 3) {
//				for (int z = 0; z < src_img.channels(); z++) {
//					dst_img.at<Vec3b>(y, x)[z] = (uchar)centers.at<float>(cluster_idx, z);
//
//				}
//			}
//			else
//			{
//				dst_img.at<uchar>(y, x) = (uchar)centers.at<float>(cluster_idx, 0);
//			}
//		}
//	}
//	return dst_img;
//}

int main() {
	Mat src_img = imread("C:\\DIP_Pic\\7\\fruits.png", 1); // 이미지 읽기
   // int flags = IMREAD_COLOR 또는 int flags = 1 -> 컬러 영상으로 읽음
   // int flags = IMREAD_GRAYSCALE 또는 int flags = 0 -> 흑백 영상으로 읽음
   // int flags = IMREAD_UNCHANGED 또는 int flags = -1 -> 원본 영상의 형식대로 읽음

	CvColorModels(src_img);
	
	////p.5 YUV 채널별 결과 확인
	//Mat YUV_img = GetYCbCr(src_img);s

	//Mat YUV[3];
	//split(YUV_img, YUV);
	//cvtColor(YUV[0], YUV[0], CV_GRAY2BGR);
	//cvtColor(YUV[1], YUV[1], CV_GRAY2BGR);
	//cvtColor(YUV[2], YUV[2], CV_GRAY2BGR);

	//imshow("yuv_y", YUV[0]);
	//imshow("yuv_u", YUV[1]);
	//imshow("yuv_v", YUV[2]);




	//waitKey(0); // 키 입력 대기(0: 키가 입력될 때 까지 프로그램 멈춤)

	//destroyWindow("Test window"); // 이미지 출력창 종료
	//imwrite("gear.jpg", doL); // 이미지 쓰기

	return 0;
}