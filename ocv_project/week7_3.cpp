#include <opencv2/opencv.hpp>
#include <iostream>
#include "opencv2/core/core.hpp" // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더
using namespace cv;
using namespace std;

void createClustersInfo(Mat imgInput, int n_cluster,
    vector<Scalar>& clustersCenters, vector<vector<Point>>& ptInclusters) {
    RNG random(getTickCount()); //OpenCV에서 무작위 값을 설정하는 함수

    for (int k = 0; k < n_cluster; k++) {//군집별 계산
        //<무작위 좌표 획득>
        Point centerKPoint;
        centerKPoint.x = random.uniform(0, imgInput.cols);
        centerKPoint.y = random.uniform(0, imgInput.rows);
        Scalar centerPixel = imgInput.at<Vec3b>(centerKPoint.y, centerKPoint.x);

        //<무작위 좌표의 화소값으로 군집별중앙값 설정>
        Scalar centerK(centerPixel.val[0], centerPixel.val[1], centerPixel.val[2]);
        clustersCenters.push_back(centerK);

        vector<Point>ptInclustersK;
        ptInclusters.push_back(ptInclustersK);
    }
}
Mat applyFinalClusterToImage(Mat src_img, int n_cluster,
    vector<vector<Point>>ptInClusters,
    vector<Scalar>clustersCenters) {

    Mat dst_img(src_img.size(), src_img.type());

    for (int k = 0; k < n_cluster; k++) {//모든 군집에 대해 수행
        vector<Point>ptInCluster = ptInClusters[k]; //군집별 좌표들
        for (int j = 0; j < ptInCluster.size(); j++) {
            //군집별 좌표 위치에 있는 화소 값을 해당 군집 중앙값으로 대체
            dst_img.at<Vec3b>(ptInCluster[j])[0] = clustersCenters[k].val[0];
            dst_img.at<Vec3b>(ptInCluster[j])[1] = clustersCenters[k].val[1];
            dst_img.at<Vec3b>(ptInCluster[j])[2] = clustersCenters[k].val[2];
        }
    }
    return dst_img;
}
Mat applyFinalClusterToImage(Mat src_img, int n_cluster,
    vector<vector<Point>>ptInClusters,
    vector < vector<Scalar>>clustersCenters) {
    Mat dst_img(src_img.size(), src_img.type());

    srand(time(NULL));
    for (int k = 0; k < n_cluster; k++) {
        vector<Point>ptInCluster = ptInClusters[k];
        Scalar randomColor(rand() % 255, rand() % 255, rand() % 255);
        for (int i = 0; i < ptInCluster.size(); i++) {
            Scalar pixelColor = dst_img.at<Vec3b>(ptInCluster[i]);
            pixelColor = randomColor;

            dst_img.at<Vec3b>(ptInCluster[i])[0] = pixelColor.val[0];
            dst_img.at<Vec3b>(ptInCluster[i])[1] = pixelColor.val[1];
            dst_img.at<Vec3b>(ptInCluster[i])[2] = pixelColor.val[2];
        }
    }
    return dst_img;
}
double computeColorDistance(Scalar pixel, Scalar clusterPixel) {
    double diffBlue = pixel.val[0] - clusterPixel[0];
    double diffGreen = pixel.val[1] - clusterPixel[1];
    double diffRed = pixel.val[2] - clusterPixel[2];

    double distance = sqrt(pow(diffBlue, 2) + pow(diffGreen, 2) + pow(diffRed, 2));
    //Euclidian distance
    return distance;
}
double adjustClusterCenters(Mat src_img, int n_cluster,
    vector<Scalar>& clustersCenters, vector<vector<Point>>ptInClusters,
    double& oldCenter, double newCenter) {

    double diffChange;

    for (int k = 0; k < n_cluster; k++) //군집별 계산
    {
        vector<Point>ptInCluster = ptInClusters[k];
        double newBlue = 0;
        double newGreen = 0;
        double newRed = 0;

        //<평균값 계산>
        for (int i = 0; i < ptInCluster.size(); i++) {
            Scalar pixel = src_img.at<Vec3b>(ptInCluster[i].y, ptInCluster[i].x);
            newBlue += pixel.val[0];
            newGreen += pixel.val[1];
            newRed += pixel.val[2];
        }
        newBlue /= ptInCluster.size();
        newGreen /= ptInCluster.size();
        newRed /= ptInCluster.size();

        //<계산한 평균값으로 군집 중앙값 대체>
        Scalar newPixel(newBlue, newGreen, newRed);
        newCenter += computeColorDistance(newPixel, clustersCenters[k]);
        //모든 군집에 대한 평균값도 같이 계산
        clustersCenters[k] = newPixel;
    }
    newCenter /= n_cluster;
    diffChange = abs(oldCenter - newCenter);
    //모든 군집에 대한 평균값 변화량 계산
    oldCenter = newCenter;

    return diffChange;
}
void findAssociatedCluster(Mat imgInput, int n_cluster,
    vector<Scalar>clustersCenters, vector<vector<Point>>& ptInClusters) {
    for (int r = 0; r < imgInput.rows; r++) {
        for (int c = 0; c < imgInput.cols; c++) {
            double minDistance = INFINITY;
            int closestClusterIndex = 0;
            Scalar pixel = imgInput.at<Vec3b>(r, c);

            for (int k = 0; k < n_cluster; k++) { //군집별 계산
                //<각 군집 중앙값과의 차이를 계산>
                Scalar clusterPixel = clustersCenters[k];
                double distance = computeColorDistance(pixel, clusterPixel);

                //<차이가 가장 적은 군집으로 좌표의 군집을 판별>
                if (distance < minDistance) {
                    minDistance = distance;
                    closestClusterIndex = k;
                }
            }
            //<좌표 저장>
            ptInClusters[closestClusterIndex].push_back(Point(c, r));
        }
    }
}
Mat MyKMeans(Mat src_img, int n_cluster) {
    vector<Scalar>clustersCenters; //군집 중앙값 벡터
    vector<vector<Point>>ptInClusters; //군집별 좌표 벡터
    double threshold = 0.001;
    double oldCenter = INFINITY;
    double newCenter = 0;
    double diffChange = oldCenter - newCenter; //군집 조정의 변화량

    //<초기설정>
    //군집 중앙값을 무작위로 할당 및 군집별 좌표값을 저장할 벡터 할당
    createClustersInfo(src_img, n_cluster, clustersCenters, ptInClusters);

    //<중앙값 조정 및 화소별 군집 판별>
    //반복적인 방법으로 군집 중앙값 조정
    //설정한 임계값 보다 군집 조정의 변화가 작을 때까지 반복
    while (diffChange > threshold) {
        //초기화
        newCenter = 0;
        for (int k = 0; k < n_cluster; k++) { ptInClusters[k].clear(); }

        //<현재의 군집 중앙값을 기준으로 군집 탐색>
        findAssociatedCluster(src_img, n_cluster, clustersCenters, ptInClusters);

        //<군집 중앙값 조절>
        diffChange = adjustClusterCenters(src_img, n_cluster, clustersCenters, ptInClusters,
            oldCenter, newCenter);
    }

    //<군집 중앙값으로만 이루어진 영상 생성>
    Mat dst_img = applyFinalClusterToImage(src_img, n_cluster, ptInClusters, clustersCenters);

    return dst_img;
}


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
            dst_img.at<Vec3b>(y, x)[0] = (uchar)(h / 2);
            dst_img.at<Vec3b>(y, x)[1] = (uchar)(s * 255);
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
    Mat src_img = imread("C:\\DIP_Pic\\7\\apple3.jpg", 1); // 이미지 읽기

    Mat myKMeansResult = MyKMeans(src_img, 4); // 직접 구현한 함수 사용

    imshow("myKMeansResult", myKMeansResult);

  //  waitKey(0); // 키 입력 대기(0: 키가 입력될 때 까지 프로그램 멈춤)
   // destroyWindow("myKMeansResult"); // 이미지 출력창 종료


   //  
    //빨강
    Scalar lower_red = Scalar(0, 50, 100);
    Scalar upper_red = Scalar(10, 255, 255);
    //파랑
    Scalar lower_blue = Scalar(100, 50, 100);
    Scalar upper_blue = Scalar(120, 255, 255);
    //초록
    Scalar lower_green = Scalar(30, 30, 50);
    Scalar upper_green = Scalar(50, 255, 255);



   //Mat src_img = imread("C:\\DIP_Pic\\7\\apple3.jpg", 1); // 이미지 읽기
     //hsv바꾸기
   Mat hsv_img = MyBgr2Hsv(myKMeansResult);

    //색별로 마스크 만들기
    Mat mask_red = MyinRange(hsv_img, lower_red, upper_red);
    Mat mask_blue = MyinRange(hsv_img, lower_blue, upper_blue);
    Mat mask_green = MyinRange(hsv_img, lower_green, upper_green);

    // 각 영역에서 가장 많은 픽셀을 가진 색상 출력
    int red_pixels = countNonZero(mask_red);
    int blue_pixels = countNonZero(mask_blue);
    int green_pixels = countNonZero(mask_green);

    Mat final;

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