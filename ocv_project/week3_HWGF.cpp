//week3_HWGF.cpp
//1��:9x9 ����þ����� ���� 
#include <iostream>
#include "opencv2/core/core.hpp" // Mat class�� ���� data structure �� ��� ��ƾ�� �����ϴ� ���
#include "opencv2/highgui/highgui.hpp" // GUI�� ���õ� ��Ҹ� �����ϴ� ���(imshow ��)
#include "opencv2/imgproc/imgproc.hpp" // ���� �̹��� ó�� �Լ��� �����ϴ� ���
using namespace cv;
using namespace std;

int myKernelConv9x9(uchar* arr, int kernel[][9], int x, int y, int width, int height) { 
    int sum = 0;
    int sumKernel = 0;

    //Ư�� ȭ���� ��� �̿�ȭ�ҿ� ���� ����ϵ��� �ݺ��� ����
    for (int j = -4; j <= 4; j++) { //9x9�� �ϱ����ؼ� ������ -4~4�� ���־���Ѵ�.
        for (int i = -4; i <= 4; i++) {
            if ((y + j) >= 0 && (y + j) < height && (x + i) >= 0 && (x + i) < width) {
                //���� �����ڸ����� ���� ���� ȭ�Ҹ� ���� �ʵ��� �ϴ� ���ǹ�
                sum += arr[(y + j) * width + (x + i)] * kernel[i + 4][j + 4];
                sumKernel += kernel[i + 4][j + 4];
            }
        }
    }
    if (sumKernel != 0) { return sum / sumKernel; } //���� 1�� ����ȭ�ǵ��� �� ������ ��⺯ȭ ����
    else { return sum; }
}

//����þ����� �Լ�
Mat mygaussianFilter(Mat srcImg) { 
    int width = srcImg.cols; //�Է¹��� �̹����� ��� ���� ���̿� ���� ����
    int height = srcImg.rows;
    int kernel[9][9] //��������� ���Ҹ� �־��ش�
        = { 1,4,7,10,7,4,1,
        4,16,26,34,26,16,4,
        7,26,41,52,41,26,7,
        10,34,51,64,52,34,10,
        16,40,59,70,59,40,13,
        10,34,51,64,52,34,10,
        7,26,41,52,41,26,7,
        4,16,26,34,26,16,4,
        1,4,7,10,7,4,1
        };

    Mat dstImg(srcImg.size(), CV_8UC1); //ä�� 1���� ���
    uchar* srcData = srcImg.data;
    uchar* dstData = dstImg.data;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            dstData[y * width + x] = myKernelConv9x9(srcData, kernel, x, y, width, height);
            //�ռ� ������ convolution�� ����ũ �迭�� �Է��� ���
        }
    }
    return dstImg;
}

int main() {
    Mat src_img = imread("C:\\DIP_Pic\\gear.jpg", 0); // �̹��� �б�
    
    // int flags = IMREAD_COLOR �Ǵ� int flags = 1 -> �÷� �������� ����
    // int flags = IMREAD_GRAYSCALE �Ǵ� int flags = 0 -> ��� �������� ����
    // int flags = IMREAD_UNCHANGED �Ǵ� int flags = -1 -> ���� ������ ���Ĵ�� ����
   

    Mat GF = mygaussianFilter(src_img);  //����þ����� ����
    imshow("Original", src_img); //���� ���� �� �̹��� ���
    imshow("Gaussian Filter", GF); //���� ���� �� �̹��� ���



    waitKey(0); // Ű �Է� ���(0: Ű�� �Էµ� �� ���� ���α׷� ����)
    destroyWindow("Test window"); // �̹��� ���â ����
    //imwrite("langding_gray.png", src_img); // �̹��� ����
    return 0;
}