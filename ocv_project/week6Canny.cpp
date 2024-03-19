#include <iostream>
#include "opencv2/core/core.hpp" // Mat class�� ���� data structure �� ��� ��ƾ�� �����ϴ� ���
#include "opencv2/highgui/highgui.hpp" // GUI�� ���õ� ��Ҹ� �����ϴ� ���(imshow ��)
#include "opencv2/imgproc/imgproc.hpp" // ���� �̹��� ó�� �Լ��� �����ϴ� ���

using namespace cv;
using namespace std;

void doCannyEx() {
	clock_t start, end; // clock_t Ÿ�� ���� ����
	start = clock(); // ���� �ð� ���
	cout << "--- doCannyEx() ---\n" << endl;
	Mat src_img = imread("C:\\DIP_Pic\\6\\edge_test.jpg", 0);
	if (!src_img.data) printf("No image data \n");

	Mat dst_img;
	
	Canny(src_img, dst_img, 500, 600);
	end = clock(); // ���� �ð� ���
	double time_taken = double(end - start) / double(CLOCKS_PER_SEC); // ���� �ð� ���

	cout << "���� �ð� : " << time_taken << "��" << endl;
	Mat result_img;
	hconcat(src_img, dst_img, result_img);

	imshow("doCannyEx()", result_img);
	waitKey(0);
}
int main() {
	// Mat src_img = imread("C:\\DIP_Pic\\6\\salt_pepper.png", 0); // �̹��� �б�
	// int flags = IMREAD_COLOR �Ǵ� int flags = 1 -> �÷� �������� ����
	// int flags = IMREAD_GRAYSCALE �Ǵ� int flags = 0 -> ��� �������� ����
	// int flags = IMREAD_UNCHANGED �Ǵ� int flags = -1 -> ���� ������ ���Ĵ�� ����

	doCannyEx();

	return 0;
}