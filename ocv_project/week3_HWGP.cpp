//week3_HWGP.cpp
//5��:�÷����� ���� Gaussian pyramid �����ϰ� ��� Ȯ��
#include <iostream>
#include "opencv2/core/core.hpp" // Mat class�� ���� data structure �� ��� ��ƾ�� �����ϴ� ���
#include "opencv2/highgui/highgui.hpp" // GUI�� ���õ� ��Ҹ� �����ϴ� ���(imshow ��)
#include "opencv2/imgproc/imgproc.hpp" // ���� �̹��� ó�� �Լ��� �����ϴ� ���
using namespace cv;
using namespace std;

//Ư�� ȭ���� ��� �̿�ȭ�ҿ� ���� ����ϵ��� �ݺ��� ����
int myKernelConv3x3(uchar* arr, int kernel[][3], int x, int y, int width, int height, int channel) {
//�÷��������� ��ȯ�ؾ��ϱ� ������ �Ű������� channel�� �߰��Ͽ��� 
    int sum = 0;
    int sumKernel = 0;
    for (int j = -1; j <= 1; j++) {//3x3�� �ϱ����ؼ� ������ -1~1�� ���־���Ѵ�.
        for (int i = -1; i <= 1; i++) {
            //3x3�� �ϱ����ؼ� ������ -1~1�� ���־���Ѵ�.
            if ((y + j) >= 0 && (y + j) < height && (x + i) >= 0 && (x + i) < width) {
                sum += arr[(y + j) * width * 3 + (x + i) * 3 + channel] * kernel[i + 1][j + 1];
                sumKernel += kernel[i + 1][j + 1];
                //�÷��� ��ȯ�ϱ� ���� 3�� ���� �÷��־���

            }
        }
    }
    if (sumKernel != 0) { return sum / sumKernel; }//���� 1�� ����ȭ�ǵ��� �� ������ ��⺯ȭ ����
    else { return sum; }
}

Mat mySampling(Mat srcImg) {
    int width = srcImg.cols / 2;
    int height = srcImg.rows / 2;
    Mat dstImg(height, width, CV_8UC3); // ä���� 3���� �ٲ㼭 �÷� �̹����� ����
    uchar* srcData = srcImg.data;
    uchar* dstData = dstImg.data;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            for (int c = 0; c < 3; c++) { // 3���� ä���� ���� ó��
                dstData[(y * width + x) * 3 + c] = srcData[((y * 2) * (width * 2) + (x * 2)) * 3 + c];
            }
        }
    }
    return dstImg;
}

Mat mygaussianFilter(Mat srcImg) {
    int width = srcImg.cols;
    int height = srcImg.rows;
    int kernel[3][3] = { {1,2,1}, {2,4,2}, {1,2,1} };
    Mat dstImg(srcImg.size(), CV_8UC3); // ä���� 3���� �ٲ㼭 �÷� �̹����� ����
    uchar* srcData = srcImg.data;
    uchar* dstData = dstImg.data;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            for (int c = 0; c < 3; c++) {
                dstData[(y * width + x) * 3 + c] = myKernelConv3x3(srcData + c, kernel, x, y, width, height, c);
            }
        }
    }
    return dstImg;
}

//down sampling�� gaussian ���� ��� smoothing�� ������ ���� ���� �ػ��� ������ �ݺ������� ����
vector<Mat> myGaussainPyramid(Mat srcImg) {
    vector<Mat> Vec;//���� ������ ��Ƽ� �����ϱ� ���� STL�� vector�����̳� ���

    Vec.push_back(srcImg); 
    for (int i = 0; i < 4; i++) {
        srcImg = mySampling(srcImg); // �ռ� ������ down sampling
        srcImg = mygaussianFilter(srcImg); //vector�����̳ʿ� �ϳ��� ó������� ����
        Vec.push_back(srcImg);
    }
    return Vec;
}

int main() {
    Mat src_img = imread("C:\\DIP_Pic\\gear.jpg", 1); // �̹��� �б�
    vector<Mat> GauPyr = myGaussainPyramid(src_img);//����þ� �Ƕ�̵� ������ ����
    // int flags = IMREAD_COLOR �Ǵ� int flags = 1 -> �÷� �������� ����
    // int flags = IMREAD_GRAYSCALE �Ǵ� int flags = 0 -> ��� �������� ����
    // int flags = IMREAD_UNCHANGED �Ǵ� int flags = -1 -> ���� ������ ���Ĵ�� ����
    
    imshow("GauPyr0", GauPyr[0]); //����þ� �Ƕ�̵带 ���� �ٸ� â���� �Ͽ� ���
    imshow("GauPyr1", GauPyr[1]); //ū â���� ��µȴ�
    imshow("GauPyr2", GauPyr[2]);
    imshow("GauPyr3", GauPyr[3]);
    imshow("GauPyr4", GauPyr[4]);

    waitKey(0); // Ű �Է� ���(0: Ű�� �Էµ� �� ���� ���α׷� ����)
    destroyWindow("Test window"); // �̹��� ���â ����
    imwrite("langding_gray.png", src_img); // �̹��� ����
    return 0;
}