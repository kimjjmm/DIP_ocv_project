#include <opencv2/opencv.hpp>
#include <iostream>
#include "opencv2/core/core.hpp" // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더
#include <opencv2/objdetect/objdetect.hpp> //객체 검출에 사용되는 함수 포함 헤더
#include <opencv2/features2d.hpp>//이미지 특징점 검출 및 매칭 함수 포함 헤더

using namespace cv;
using namespace std;


void cvHarrisCornerandPerspective() { //블러처리
    // 이미지 불러오기
    Mat img = imread("C:\\DIP_Pic\\12\\card_per.png");
    Mat src = img;
    // 이미지 없는 경우 예외
    if (img.empty()) {
        cout << "Empty image!" << endl;
        exit(-1);
    }
    // 이미지 크기 조절
    resize(img, img, Size(500, 500), 0, 0, INTER_CUBIC);

    // 그레이 스케일 이미지 생성
    Mat gray;
    cvtColor(img, gray, CV_BGR2GRAY);

    // 이미지 블러 처리
    Mat blurred;
    GaussianBlur(gray, blurred, Size(11, 31), 0);

    // 해리스 코너 검출 수행
    Mat harris;
    cornerHarris(blurred, harris, 21, 21, 0.04, BORDER_DEFAULT);

    // 결과 이미지 normalize
    normalize(harris, harris, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
    Mat harris_abs;
    convertScaleAbs(harris, harris_abs);


    // 4분면 영역별로 x, y가 가장 작은 코너 저장을 위한 변수
    Point min_corner_tl;  // Top-Left
    Point min_corner_tr;  // Top-Right
    Point min_corner_bl;  // Bottom-Left
    Point min_corner_br;  // Bottom-Right

    // 임계값 기준으로 코너에 원 그리기 및 최소값 저장
    int thresh = 190; //임계값 설정
    Mat result = img.clone(); //입력이미지의 복사본으로 초기화 
    for (int y = 0; y < harris.rows; y += 1) { //harris의 픽셀 순회
        for (int x = 0; x < harris.cols; x += 1) {
            if ((int)harris.at<float>(y, x) > thresh) { //해당 픽셀값이 임계값보다 큰지 판별
                circle(result, Point(x, y), 30, Scalar(0, 0, 0), 0, 4, 0); //원 그리기

                //코너들 중에서 
                //이미지를 4분면 영역으로 나누어 이미지의 네 꼭짓점만 좌표 값 저장
                if (x < harris.cols / 2 && y < harris.rows / 2) { //1사분면
                    if (min_corner_tl.x == 0 || (x < min_corner_tl.x && y < min_corner_tl.y))
                        min_corner_tl = Point(x, y);
                }
                else if (x >= harris.cols / 2 && y < harris.rows / 2) { //2사분면
                    if (min_corner_tr.x == 0 || (x < min_corner_tr.x && y < min_corner_tr.y))
                        min_corner_tr = Point(x, y);
                }
                else if (x < harris.cols / 2 && y >= harris.rows / 2) {//3사분면
                    if (min_corner_bl.x == 0 || (x < min_corner_bl.x && y > min_corner_bl.y))
                        min_corner_bl = Point(x, y);
                }
                else { //4사분면
                    if (min_corner_br.x == 0 || (x > min_corner_br.x && y > min_corner_br.y))
                        min_corner_br = Point(x, y);
                }
            }
        }
    }

    // 각 영역에서 x와 y값을 통해 네 꼭짓점만 출력
    cout << "Top-Left: (" << min_corner_tl.x << ", " << min_corner_tl.y << ")" << endl;
    cout << "Top-Right: (" << min_corner_tr.x << ", " << min_corner_tr.y << ")" << endl;
    cout << "Bottom-Left: (" << min_corner_bl.x << ", " << min_corner_bl.y << ")" << endl;
    cout << "Bottom-Right: (" << min_corner_br.x << ", " << min_corner_br.y << ")" << endl;

    // 이미지 출력
    imshow("Source image", img);          // 입력 이미지
    imshow("Blurred image", blurred);     // 블러 처리된 이미지
    imshow("Harris image", harris_abs);   // 해리스 코너 시각화 결과
    imshow("Target image", result);       // 원 검출 결과

    // 네 개의 코너를 이미지에 나타내기
    circle(img, min_corner_tl, 30, Scalar(255, 0, 0), -1);  // Blue (BGR)
    circle(img, min_corner_tr, 30, Scalar(0, 255, 0), -1);  // Green
    circle(img, min_corner_bl, 30, Scalar(0, 0, 255), -1);  // Red
    circle(img, min_corner_br, 30, Scalar(255, 255, 0), -1);  // Cyan

    // 이미지 출력
    imshow("4corners", img);

    Mat dst, matrix;

    Point2f srcQuad[4]; //변환 전 좌표
    srcQuad[0] = min_corner_tl;
    srcQuad[1] = min_corner_tr;
    srcQuad[2] = min_corner_bl;
    srcQuad[3] = min_corner_br;

    Point2f dstQuad[4]; //변환 후 좌표
    dstQuad[0] = Point2f(src.cols * 0.1f, src.rows * 0.25f);
    dstQuad[1] = Point2f(src.cols * 0.9f, src.rows * 0.265f);
    dstQuad[2] = Point2f(src.cols * 0.1f, src.rows * 0.75f);
    dstQuad[3] = Point2f(src.cols * 0.9f, src.rows * 0.75f);
    
    matrix = getPerspectiveTransform(srcQuad, dstQuad); //원근변환 행렬
    warpPerspective(src, dst, matrix, img.size()); //원근 변환 적용하여 dst에 결과 이미지 저장
    
    //이미지 저장
    imwrite("nonper.jpg", src); //변환 전
    imwrite("per.jpg", dst); //변환 후

    //이미지 출력
    imshow("nonper", src); //변환 전
    imshow("per", dst); //변환 후
    waitKey(0);

    destroyAllWindows();
}



int main() {
   //myHarrisCorner();
    cvHarrisCornerandPerspective();
   // cvPerspective();
}