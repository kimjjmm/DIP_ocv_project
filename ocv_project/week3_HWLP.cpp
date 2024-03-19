//week3_HWGP.cpp
//6��:�÷����� ���� Laplacian pyramid �����ϰ� ��� Ȯ��
#include <opencv2/opencv.hpp>
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
//laplacian pyramid����
vector<Mat> myLaplacianPyramid(Mat srcImg) {
    vector<Mat> Vec;
    for (int i = 0; i < 4; i++) {
        if (i != 3) {
            Mat highImg = srcImg; //�����ϱ� ���� ������ ���
            srcImg = mySampling(srcImg);
            srcImg = mygaussianFilter(srcImg);
            Mat lowImg = srcImg;
            resize(lowImg, lowImg, highImg.size()); //�۾��� ������ ����� ������ ũ��� Ȯ��
            Vec.push_back(highImg - lowImg + 128);
            //�����̳ʿ� ����. 128�� ���� ���� �����÷ο츦 �����ϱ� ����
        }
        else {
            Vec.push_back(srcImg);
        }
    }
    return Vec;
}
int main() {
    Mat src_img = imread("C:\\DIP_Pic\\gear.jpg", 1); // �̹��� �б�
    vector<Mat> VecLap = myLaplacianPyramid(src_img); //laplacian pyramidȮ��
    // int flags = IMREAD_COLOR �Ǵ� int flags = 1 -> �÷� �������� ����
    // int flags = IMREAD_GRAYSCALE �Ǵ� int flags = 0 -> ��� �������� ����
    // int flags = IMREAD_UNCHANGED �Ǵ� int flags = -1 -> ���� ������ ���Ĵ�� ����
    imshow("VecLap0", VecLap[0]);
    imshow("VecLap1", VecLap[1]);
    imshow("VecLap2", VecLap[2]);
    imshow("VecLap3", VecLap[3]);
    waitKey(0);
    destroyWindow("VecLap3");

    reverse(VecLap.begin(), VecLap.end());
    //���� ������� ó���ϱ� ���� vector������ �ݴ�� 

    Mat dst_img;
    for (int i = 0; i < VecLap.size(); i++) {
        //vector�� ũ�⸸ŭ �ݺ�
        if (i == 0) {
            dst_img = VecLap[i];
           
        }
        else {
            resize(dst_img, dst_img, VecLap[i].size());
            dst_img = dst_img + VecLap[i] - 128;
            //���󤷸� �ٽ� ���� ū �������� ����
            //�����÷ο� ���������� ���ߴ� 128�� �ٽ� ����
        }
        string fname = "ex5_lap_pyr" + to_string(i) + ".png";
        imwrite(fname, dst_img);
        imshow("EX5", dst_img);

        waitKey(0);
        destroyWindow("EX5");
    }
    return 0;
}