//sobel frequency domain 12205102������

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
Mat doDft(Mat srcImg) {//�����ȣ�� ���ļ� �������� �ٲٴ� �Լ�
    Mat floatImg;
    srcImg.convertTo(floatImg, CV_32F); //32bit floatning point number�� convert
    Mat complexImg;
    dft(floatImg, complexImg, DFT_COMPLEX_OUTPUT); //DFT_COMPLEX_OUTPUT�� �������� ���Ҽ��� ��

    return complexImg;
}
Mat getMagnitude(Mat complexImg) { //magnitude ���� ���
    Mat planes[2];
    split(complexImg, planes);// �Ǽ���, ����� �и�

    Mat magImg;
    magnitude(planes[0], planes[1], magImg);
    magImg += Scalar::all(1);
    log(magImg, magImg); //log scale ��ȯ 
    //magnitude ���

    return magImg;
}
Mat getPhase(Mat complexImg) { //phase ���� ���
    Mat planes[2];
    split(complexImg, planes); //�Ǽ���, ����� �и�
    Mat phaImg;
    phase(planes[0], planes[1], phaImg);
    return phaImg;
}
Mat myNormalize(Mat src) {//����ȭ
    Mat dst;
    src.copyTo(dst);
    normalize(dst, dst, 0, 255, NORM_MINMAX);
    //��ȯ�ּҰ� 0���� ��ȯ�ִ밪 255������ ��ȯ. NORM_MINMAX�� ��ȯ ���.
    dst.convertTo(dst, CV_8UC1); //0~255. ����̹�����

    return dst;
}
Mat centralize(Mat complex) {//��ǥ�� �߾� �̵�

    Mat planes[2];
    split(complex, planes);
    int cx = planes[0].cols / 2;
    int cy = planes[1].rows / 2;

    Mat q0Re(planes[0], Rect(0, 0, cx, cy));
    Mat q1Re(planes[0], Rect(cx, 0, cx, cy));
    Mat q2Re(planes[0], Rect(0, cy, cx, cy));
    Mat q3Re(planes[0], Rect(cx, cy, cx, cy));

    Mat tmp; //�ӽ�����
    q0Re.copyTo(tmp);
    q3Re.copyTo(q0Re);
    tmp.copyTo(q3Re);
    q1Re.copyTo(tmp);
    q2Re.copyTo(q1Re);
    tmp.copyTo(q2Re);

    Mat q0Im(planes[1], Rect(0, 0, cx, cy));
    Mat q1Im(planes[1], Rect(cx, 0, cx, cy));
    Mat q2Im(planes[1], Rect(0, cy, cx, cy));
    Mat q3Im(planes[1], Rect(cx, cy, cx, cy));

    q0Im.copyTo(tmp);
    q3Im.copyTo(q0Im);
    tmp.copyTo(q3Im);
    q1Im.copyTo(tmp);
    q2Im.copyTo(q1Im);
    tmp.copyTo(q2Im);

    Mat centerComplex;
    merge(planes, 2, centerComplex);
    return centerComplex;
}
Mat padding(Mat img) {
    int dftRows = getOptimalDFTSize(img.rows);
    int dftCols = getOptimalDFTSize(img.cols);

    Mat padded;
    copyMakeBorder(img, padded, 0, dftRows - img.rows, 0, dftCols - img.cols, BORDER_CONSTANT, Scalar::all(0));
    return padded;
}

int main() {
    Mat src_img = imread("C:\\DIP_Pic\\5\\img2.jpg", 0); // �̹��� �б�
    // int flags = IMREAD_COLOR �Ǵ� int flags = 1 -> �÷� �������� ����
    // int flags = IMREAD_GRAYSCALE �Ǵ� int flags = 0 -> ��� �������� ����
    // int flags = IMREAD_UNCHANGED �Ǵ� int flags = -1 -> ���� ������ ���Ĵ�� ����

    Mat sobel = mysobelFilter(src_img);//�����̹����� sobel���� ����
    imshow("sobelimg", sobel); //sobel���� �̹��� ���
    imshow("origimg", src_img);

    //dft_origin
    Mat padd_orig = padding(src_img);

    Mat freq_orig = doDft(padd_orig);
    Mat cent_orig = centralize(freq_orig);

    Mat mag_orig = getMagnitude(cent_orig); //magnitude�̹���
    Mat origmag = myNormalize(mag_orig);

    Mat pha_orig = getPhase(cent_orig); //phase�̹���
    Mat origpha = myNormalize(pha_orig);

    imshow("origlmag", origmag); //���������� magnitude�̹��� ���	
    imshow("origpha", origpha); //���������� phase�̹��� ���	

    //dft_sobel
    Mat padd_sobel = padding(sobel);

    Mat freq_sobel = doDft(padd_sobel);
    Mat cent_sobel = centralize(freq_sobel);

    Mat mag_sobel = getMagnitude(cent_sobel); //magnitude�̹���
    Mat sobelmag = myNormalize(mag_sobel);

    Mat pha = getPhase(cent_sobel); //phase�̹���
    Mat sobelpha = myNormalize(pha);


    imshow("sobelmag", sobelmag); //sobel���� ���� �� magnitude�̹��� ���	
    imshow("sobelpha", sobelpha); //sobel���� ���� �� phase�̹��� ���	


    waitKey(0); // Ű �Է� ���(0: Ű�� �Էµ� �� ���� ���α׷� ����)
    destroyWindow("Test window"); // �̹��� ���â ����

    return 0;
}