//week3_HWSob.cpp
//4��:45���� 135���� �밢���� �����ϴ� sobel filter����
#include <iostream>
#include "opencv2/core/core.hpp" // Mat class�� ���� data structure �� ��� ��ƾ�� �����ϴ� ���
#include "opencv2/highgui/highgui.hpp" // GUI�� ���õ� ��Ҹ� �����ϴ� ���(imshow ��)
#include "opencv2/imgproc/imgproc.hpp" // ���� �̹��� ó�� �Լ��� �����ϴ� ���
using namespace cv;
using namespace std;


//Ư�� ȭ���� ��� �̿�ȭ�ҿ� ���� ����ϵ��� �ݺ��� ����
int myKernelConv3x3(uchar* arr, int kernel[][3], int x, int y, int width, int height) {
    int sum = 0;
    int sumKernel = 0;
    for (int j = -1; j <= 1; j++) { //3x3�� �ϱ����ؼ� ������ -1~1�� ���־���Ѵ�.
        for (int i = -1; i <= 1; i++) {
            //���� �����ڸ����� ���� ���� ȭ�Ҹ� ���� �ʵ��� �ϴ� ���ǹ�
            if ((y + j) >= 0 && (y + j) < height && (x + i) >= 0 && (x + i) < width) {
                sum += arr[(y + j) * width + (x + i)] * kernel[i + 1][j + 1];
                sumKernel += kernel[i + 1][j + 1];
            }
        }
    }
    if (sumKernel != 0) { return sum / sumKernel; } //���� 1�� ����ȭ�ǵ��� �� ������ ��⺯ȭ ����
    else { return sum; }
}

Mat mysobelFilter(Mat srcImg) {
    
    //����ũ�� ���� ����
    int kernelX[3][3] = { 0, 1 ,2, // 45�� �밢����
                         -1, 0, 1,
                         -2, -1, 0 };
    int kernelY[3][3] = { -2,-1,1, //135�� �밢����
                         -1, 0, 1,
                          0, 1, 2 };

    Mat dstImg(srcImg.size(), CV_8UC1); //ä��1�� ���
    uchar* srcData = srcImg.data;
    uchar* dstData = dstImg.data;
    int width = srcImg.cols;
    int height = srcImg.rows;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            dstData[y * width + x] = (abs(myKernelConv3x3(srcData, kernelX, x, y, width, height)) +
                abs(myKernelConv3x3(srcData, kernelY, x, y, width, height))) / 2;
            //�� ���� ����� ���밪 �� ���·� ������� ����
        }
    }
    return dstImg;
}


int main() {
    Mat src_img = imread("C:\\DIP_Pic\\gear.jpg", 0); // �̹��� �б�
    Mat sobel = mysobelFilter(src_img);
    // int flags = IMREAD_COLOR �Ǵ� int flags = 1 -> �÷� �������� ����
    // int flags = IMREAD_GRAYSCALE �Ǵ� int flags = 0 -> ��� �������� ����
    // int flags = IMREAD_UNCHANGED �Ǵ� int flags = -1 -> ���� ������ ���Ĵ�� ����

    imshow("soble", sobel);
    waitKey(0); // Ű �Է� ���(0: Ű�� �Էµ� �� ���� ���α׷� ����)
    destroyWindow("Test window"); // �̹��� ���â ����
    imwrite("langding_gray.png", src_img); // �̹��� ����
    return 0;
}