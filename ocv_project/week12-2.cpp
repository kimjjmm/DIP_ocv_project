#include <opencv2/opencv.hpp>
#include <iostream>
#include "opencv2/core/core.hpp" // Mat class�� ���� data structure �� ��� ��ƾ�� �����ϴ� ���
#include "opencv2/highgui/highgui.hpp" // GUI�� ���õ� ��Ҹ� �����ϴ� ���(imshow ��)
#include "opencv2/imgproc/imgproc.hpp" // ���� �̹��� ó�� �Լ��� �����ϴ� ���
#include <opencv2/objdetect/objdetect.hpp> //��ü ���⿡ ���Ǵ� �Լ� ���� ���
#include <opencv2/features2d.hpp>//�̹��� Ư¡�� ���� �� ��Ī �Լ� ���� ���

using namespace cv;
using namespace std;


void cvHarrisCornerandPerspective() { //��ó��
    // �̹��� �ҷ�����
    Mat img = imread("C:\\DIP_Pic\\12\\card_per.png");
    Mat src = img;
    // �̹��� ���� ��� ����
    if (img.empty()) {
        cout << "Empty image!" << endl;
        exit(-1);
    }
    // �̹��� ũ�� ����
    resize(img, img, Size(500, 500), 0, 0, INTER_CUBIC);

    // �׷��� ������ �̹��� ����
    Mat gray;
    cvtColor(img, gray, CV_BGR2GRAY);

    // �̹��� �� ó��
    Mat blurred;
    GaussianBlur(gray, blurred, Size(11, 31), 0);

    // �ظ��� �ڳ� ���� ����
    Mat harris;
    cornerHarris(blurred, harris, 21, 21, 0.04, BORDER_DEFAULT);

    // ��� �̹��� normalize
    normalize(harris, harris, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
    Mat harris_abs;
    convertScaleAbs(harris, harris_abs);


    // 4�и� �������� x, y�� ���� ���� �ڳ� ������ ���� ����
    Point min_corner_tl;  // Top-Left
    Point min_corner_tr;  // Top-Right
    Point min_corner_bl;  // Bottom-Left
    Point min_corner_br;  // Bottom-Right

    // �Ӱ谪 �������� �ڳʿ� �� �׸��� �� �ּҰ� ����
    int thresh = 190; //�Ӱ谪 ����
    Mat result = img.clone(); //�Է��̹����� ���纻���� �ʱ�ȭ 
    for (int y = 0; y < harris.rows; y += 1) { //harris�� �ȼ� ��ȸ
        for (int x = 0; x < harris.cols; x += 1) {
            if ((int)harris.at<float>(y, x) > thresh) { //�ش� �ȼ����� �Ӱ谪���� ū�� �Ǻ�
                circle(result, Point(x, y), 30, Scalar(0, 0, 0), 0, 4, 0); //�� �׸���

                //�ڳʵ� �߿��� 
                //�̹����� 4�и� �������� ������ �̹����� �� �������� ��ǥ �� ����
                if (x < harris.cols / 2 && y < harris.rows / 2) { //1��и�
                    if (min_corner_tl.x == 0 || (x < min_corner_tl.x && y < min_corner_tl.y))
                        min_corner_tl = Point(x, y);
                }
                else if (x >= harris.cols / 2 && y < harris.rows / 2) { //2��и�
                    if (min_corner_tr.x == 0 || (x < min_corner_tr.x && y < min_corner_tr.y))
                        min_corner_tr = Point(x, y);
                }
                else if (x < harris.cols / 2 && y >= harris.rows / 2) {//3��и�
                    if (min_corner_bl.x == 0 || (x < min_corner_bl.x && y > min_corner_bl.y))
                        min_corner_bl = Point(x, y);
                }
                else { //4��и�
                    if (min_corner_br.x == 0 || (x > min_corner_br.x && y > min_corner_br.y))
                        min_corner_br = Point(x, y);
                }
            }
        }
    }

    // �� �������� x�� y���� ���� �� �������� ���
    cout << "Top-Left: (" << min_corner_tl.x << ", " << min_corner_tl.y << ")" << endl;
    cout << "Top-Right: (" << min_corner_tr.x << ", " << min_corner_tr.y << ")" << endl;
    cout << "Bottom-Left: (" << min_corner_bl.x << ", " << min_corner_bl.y << ")" << endl;
    cout << "Bottom-Right: (" << min_corner_br.x << ", " << min_corner_br.y << ")" << endl;

    // �̹��� ���
    imshow("Source image", img);          // �Է� �̹���
    imshow("Blurred image", blurred);     // �� ó���� �̹���
    imshow("Harris image", harris_abs);   // �ظ��� �ڳ� �ð�ȭ ���
    imshow("Target image", result);       // �� ���� ���

    // �� ���� �ڳʸ� �̹����� ��Ÿ����
    circle(img, min_corner_tl, 30, Scalar(255, 0, 0), -1);  // Blue (BGR)
    circle(img, min_corner_tr, 30, Scalar(0, 255, 0), -1);  // Green
    circle(img, min_corner_bl, 30, Scalar(0, 0, 255), -1);  // Red
    circle(img, min_corner_br, 30, Scalar(255, 255, 0), -1);  // Cyan

    // �̹��� ���
    imshow("4corners", img);

    Mat dst, matrix;

    Point2f srcQuad[4]; //��ȯ �� ��ǥ
    srcQuad[0] = min_corner_tl;
    srcQuad[1] = min_corner_tr;
    srcQuad[2] = min_corner_bl;
    srcQuad[3] = min_corner_br;

    Point2f dstQuad[4]; //��ȯ �� ��ǥ
    dstQuad[0] = Point2f(src.cols * 0.1f, src.rows * 0.25f);
    dstQuad[1] = Point2f(src.cols * 0.9f, src.rows * 0.265f);
    dstQuad[2] = Point2f(src.cols * 0.1f, src.rows * 0.75f);
    dstQuad[3] = Point2f(src.cols * 0.9f, src.rows * 0.75f);
    
    matrix = getPerspectiveTransform(srcQuad, dstQuad); //���ٺ�ȯ ���
    warpPerspective(src, dst, matrix, img.size()); //���� ��ȯ �����Ͽ� dst�� ��� �̹��� ����
    
    //�̹��� ����
    imwrite("nonper.jpg", src); //��ȯ ��
    imwrite("per.jpg", dst); //��ȯ ��

    //�̹��� ���
    imshow("nonper", src); //��ȯ ��
    imshow("per", dst); //��ȯ ��
    waitKey(0);

    destroyAllWindows();
}



int main() {
   //myHarrisCorner();
    cvHarrisCornerandPerspective();
   // cvPerspective();
}