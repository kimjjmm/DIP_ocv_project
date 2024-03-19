#include <opencv2/opencv.hpp>
#include <iostream>
#include "opencv2/core/core.hpp" // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더
using namespace cv;
using namespace std;

Mat CvKMeans(Mat src_img, int k) {
    Mat samples(src_img.rows * src_img.cols, src_img.channels(), CV_32F);
    for (int y = 0;  y < src_img.rows; y++) {
        for (int x = 0; x < src_img.cols; x++) {
            if (src_img.channels() == 3) {
                for (int z = 0; z < src_img.channels(); z++) {
                    samples.at<float>(y + x * src_img.rows, z) =
                        (float)src_img.at<Vec3b>(y, x)[z];
                }
            }
            else {
                samples.at<float>(y + x * src_img.rows) =
                    (float)src_img.at<uchar>(y, x);
            }
        }
    }
    Mat labels;
    Mat centers;
    int attempts = 5;
    kmeans(samples, k, labels, TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 
        10000, 0.0001), attempts, KMEANS_PP_CENTERS, centers);

    Mat dst_img(src_img.size(), src_img.type());
    for (int y = 0; y < src_img.rows; y++) {
        for (int x = 0; x < src_img.cols; x++) {
            int cluster_idx = labels.at<int>(y + x * src_img.rows, 0);
            if (src_img.channels() == 3) {
                for (int z = 0; z < src_img.channels(); z++) {
                    dst_img.at<Vec3b>(y, x)[z] =
                        (uchar)centers.at<float>(cluster_idx, z);
                }
            }
            else {
                dst_img.at<uchar>(y, x) =
                    (uchar)centers.at<float>(cluster_idx, 0);
            }
        }
    }
    return dst_img;
}

int main() {
    Mat src_img = imread("C:\\DIP_Pic\\7\\beach.jpg", 1);
    Mat beach2 = CvKMeans(src_img, 2); //2개로 clustering
    Mat beach4 = CvKMeans(src_img, 4); //4개로 clustering
    Mat beach8 = CvKMeans(src_img, 8); //8개로 clustering

    imshow("beach2", beach2); 
    imshow("beach4", beach4);
    imshow("beach8", beach8);
    waitKey(0); // 키 입력 대기(0: 키가 입력될 때 까지 프로그램 멈춤

    return 0;
}