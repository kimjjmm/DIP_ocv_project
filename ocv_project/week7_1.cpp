#include <opencv2/opencv.hpp>
#include <iostream>
#include "opencv2/core/core.hpp" // Mat class�� ���� data structure �� ��� ��ƾ�� �����ϴ� ���
#include "opencv2/highgui/highgui.hpp" // GUI�� ���õ� ��Ҹ� �����ϴ� ���(imshow ��)
#include "opencv2/imgproc/imgproc.hpp" // ���� �̹��� ó�� �Լ��� �����ϴ� ���
using namespace cv;
using namespace std;

Mat MyBgr2Hsv(Mat src_img) { 
    //BGR�� �������� ǥ���� �̹����� HSV�� ��ȯ
    double b, g, r, h, s, v;
    Mat dst_img(src_img.size(), src_img.type());
    //�Է��̹����� ���� ũ��� Ÿ���� ������ ����̹��� ����
    for (int y = 0; y < src_img.rows; y++) {
        for (int x = 0; x < src_img.cols; x++) {
            b = (double)src_img.at<Vec3b>(y, x)[0];
            g = (double)src_img.at<Vec3b>(y, x)[1];
            r = (double)src_img.at<Vec3b>(y, x)[2];
            //��� �ȼ��� ���� �ݺ��ϰ� �ȼ��� BGR���� b,g,r������ ����

         
            // BGR ���� HSV ������ ��ȯ
            double max_val = max(max(b, g), r);
            double min_val = min(min(b, g), r);
            double delta = max_val - min_val; 
            //delta: �ִ밪 - �ּҰ� ����

            //Hue ���
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
                h = h + 360; // hue�� �����̸�, h +30���� hue�� �ʱ�ȭ
            }

            //Saturation ���
            if (delta == 0) { //max-min = 0
                s = 0;
            }
            else
                s = (max_val - min_val) / max_val;

            //Value���
            v = max_val; //value���� ��⸦ ��Ÿ���� �����μ�, �ȼ� �� ���� ū ���� max_val�� value�� ��
            
           // Mat hsv_img;
            dst_img.at<Vec3b>(y, x)[0] = (uchar)(h/2);
            dst_img.at<Vec3b>(y, x)[1] = (uchar)(s*255);
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
            //��� �ȼ��� ���� �ݺ��ϰ� ���� bgr�� ���� ����

            //bgr�� ������
            bool in_range = (lower[0] <= b && b <= upper[0]) &&
                (lower[1] <= g && g <= upper[1]) &&
                (lower[2] <= r && r <= upper[2]);
            if (in_range) {//lower-upper ���� ���� �ִ� �ȼ��� ���
                dst_img.at<uchar>(y, x) = 255;
            }
            else {//���� �ܿ� �ִ� �ȼ��� ������
                dst_img.at<uchar>(y, x) = 0;
            }

        }
    }
    return dst_img;
}


int main() {
    //����
    Scalar lower_red = Scalar(0, 50, 100);
    Scalar upper_red = Scalar(10, 255, 255);
    //�Ķ�
    Scalar lower_blue = Scalar(100, 50, 100);
    Scalar upper_blue = Scalar(120, 255, 255);
    //�ʷ�
    Scalar lower_green = Scalar(30, 30, 50);
    Scalar upper_green = Scalar(50, 255, 255);



    Mat src_img = imread("C:\\DIP_Pic\\7\\apple.jpg", 1); // �̹��� �б�
     //hsv�ٲٱ�
    Mat hsv_img = MyBgr2Hsv(src_img);

    //������ ����ũ �����
    Mat mask_red = MyinRange(hsv_img, lower_red, upper_red);
    Mat mask_blue = MyinRange(hsv_img, lower_blue, upper_blue);
    Mat mask_green = MyinRange(hsv_img, lower_green, upper_green);

    // �� �������� ���� ���� �ȼ��� ���� ���� ���
    int red_pixels = countNonZero(mask_red);
    int blue_pixels = countNonZero(mask_blue);
    int green_pixels = countNonZero(mask_green);

    Mat final;

    //�̹������� ���� ���� �ȼ����� ���� ���� ã��
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

    waitKey(0); // Ű �Է� ���(0: Ű�� �Էµ� �� ���� ���α׷� ����)
    
    return 0;
}
