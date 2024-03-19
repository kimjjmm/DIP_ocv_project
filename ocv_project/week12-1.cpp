#include <opencv2/opencv.hpp>
#include <iostream>
#include "opencv2/core/core.hpp" // Mat class�� ���� data structure �� ��� ��ƾ�� �����ϴ� ���
#include "opencv2/highgui/highgui.hpp" // GUI�� ���õ� ��Ҹ� �����ϴ� ���(imshow ��)
#include "opencv2/imgproc/imgproc.hpp" // ���� �̹��� ó�� �Լ��� �����ϴ� ���
#include <iostream>
#include <cmath>

using namespace cv;
using namespace std;


struct Point { //point ����ü ����
    int x;
    int y;
    Point(int x, int y) : x(x), y(y) {} //����ü ������
};
struct Size { //size ����ü ����
    int width;
    int height;
    Size(int width, int height) : width(width), height(height) {} //����ü ������
};

//�̹��� ȸ��
void getMyRotation(const Mat& src, Mat& dst, double angle) {
    cv::Point center(src.cols / 2, src.rows / 2); //�̹��� �߽���ǥ ���ϱ�
    double radian = angle * CV_PI / 180.0; //���� ������ ��ȯ
    double cos_val = cos(radian); //cos �� ���
    double sin_val = sin(radian); //sin �� ���

    //ȸ�� ��ȯ ���
    double rotation_matrix_data[] = { cos_val, -sin_val, (1 - cos_val) * center.x + sin_val * center.y,
                                      sin_val, cos_val, -sin_val * center.x + (1 - cos_val) * center.y };

    Mat rotation_matrix(2, 3, CV_64F, rotation_matrix_data);

    //ȸ�� ��ȯ ����
    warpAffine(src, dst, rotation_matrix, src.size());

}


int main() {

    Mat src = imread("C:\\DIP_Pic\\12\\Lenna.png", 1); //�̹��� �б�
    Mat dst;

    double angle = -45.0; //ȸ���� ����(�ݽð�������� 45��). �ð�������� ȸ���ϰ� �ʹٸ� 45�� �Է��ϸ� ��
    getMyRotation(src, dst, angle);//�̹��� ȸ��

    //�̹��� ���� 
    imwrite("nonrot.jpg", src); //ȸ�� ��
    imwrite("rot.jpg", dst); //ȸ�� ��

    //�̹��� ���
    imshow("nonrot", src); //ȸ�� ��
    imshow("rot", dst); //ȸ�� ��
    waitKey(0);

    destroyAllWindows();

    return 0;
}
