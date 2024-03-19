#include <iostream>
#include "opencv2/core/core.hpp" // Mat class�� ���� data structure �� ��� ��ƾ�� �����ϴ� ���
#include "opencv2/highgui/highgui.hpp" // GUI�� ���õ� ��Ҹ� �����ϴ� ���(imshow ��)
#include "opencv2/imgproc/imgproc.hpp" // ���� �̹��� ó�� �Լ��� �����ϴ� ���

using namespace cv;
using namespace std;

void myMedian(const Mat& src_img, Mat& dst_img, const Size& kn_size) {
	dst_img = Mat::zeros(src_img.size(), CV_8UC1);
	int wd = src_img.cols; //�Է��̹��� �ʺ�� �ʱ�ȭ
	int hg = src_img.rows; //�Է��̹��� ���̷� �ʱ�ȭ
	int kwd = kn_size.width;  //Ŀ�γʺ� �ʱ�ȭ
	int khg = kn_size.height; //Ŀ�γ��� �ʱ�ȭ
	int rad_w = kwd / 2; //�߽��ȼ��� �������� ���Ʒ� �翷���� �󸶳� �������ִ���
	int rad_h = khg / 2;
	uchar* src_data = (uchar*)src_img.data;
	uchar* dst_data = (uchar*)dst_img.data;
	float* table = new float[kwd * khg](); //Ŀ�����̺� �����Ҵ�
	float tmp;

	//�ȼ� �ε���(�����ڸ� ����)
	for (int c = rad_w + 1; c < wd - rad_w; c++) { //������. col idx
		for (int r = rad_h + 1; r < hg - rad_h; r++) { //������. row idx
			
		 //Ŀ�� ���̺� �ʱ�ȭ
			int index = 0;
			for (int i = -rad_w; i <= rad_w; i++) {
				for (int j = -rad_h; j <= rad_h; j++) {
					tmp = src_data[(r + j) * wd + c + i]; //�߽��ȼ��������� Ŀ�����̺����� �ε��� ���
					table[index++] = tmp; //�ε����� �ش��ϴ� �ȼ��� ����
				}
			}

			// �߰� �� ã��
			std::sort(table, table + kwd * khg); //�������� ����
			uchar median = table[kwd * khg / 2]; //�߰��� ����

			// Salt and Pepper Noise ����
			if (median == 0 || median == 255) {//�߰����� 0Ȥ�� 255�� ���
				dst_data[r * wd + c] = src_data[r * wd + c]; // ���� �̹��� ������ ��ü
			}
			else {
				dst_data[r * wd + c] = median; //�߰������� ��ü
			}
		}
	}
	delete table; //�����Ҵ� ����
}
void doMedianEx() {
	cout << "--- doMedianEx() --- \n" << endl;
	//�Է�
	Mat src_img = imread("C:\\DIP_Pic\\6\\salt_pepper.png", 0);
	if (!src_img.data) printf("No image data \n");
	//median ���͸�����
	Mat dst_img;
	myMedian(src_img, dst_img, Size(3, 3));
	//���
	Mat result_img;
	hconcat(src_img, dst_img, result_img);
	imshow("doMedianEx()", result_img);

}

//5x5���͸��񱳸� ���� �ۼ��Ͽ���
void doMedianEx2() {
	cout << "--- doMedianEx() --- \n" << endl;
	//�Է�
	Mat src_img = imread("C:\\DIP_Pic\\6\\salt_pepper.png", 0);
	if (!src_img.data) printf("No image data \n");
	//median ���͸�����
	Mat dst_img;
	myMedian(src_img, dst_img, Size(5, 5));
	//���
	Mat result_img;
	hconcat(src_img, dst_img, result_img);
	imshow("doMedianEx2", result_img);
	waitKey(0);
}

int main() {
	//Mat src_img = imread("C:\\DIP_Pic\\6\\salt_pepper.png", 0); // �̹��� �б�
	// int flags = IMREAD_COLOR �Ǵ� int flags = 1 -> �÷� �������� ����
	// int flags = IMREAD_GRAYSCALE �Ǵ� int flags = 0 -> ��� �������� ����
	// int flags = IMREAD_UNCHANGED �Ǵ� int flags = -1 -> ���� ������ ���Ĵ�� ����


	doMedianEx(); //median ���� ����
	doMedianEx2(); //median ���� ����

	waitKey(0); // Ű �Է� ���(0: Ű�� �Էµ� �� ���� ���α׷� ����)
	destroyWindow("Test window"); // �̹��� ���â ����
	//imwrite("gear.jpg", doL); // �̹��� ����

	return 0;
}