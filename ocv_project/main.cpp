
//HW img1

#include <iostream>
#include<cstdlib>
#include<time.h>
#include "opencv2/core/core.hpp" // Mat class�� ���� data structure �� ��� ��ƾ�� �����ϴ� ���
#include "opencv2/highgui/highgui.hpp" // GUI�� ���õ� ��Ҹ� �����ϴ� ���(imshow ��)
#include "opencv2/imgproc/imgproc.hpp" // ���� �̹��� ó�� �Լ��� �����ϴ� ���

using namespace cv;
using namespace std;


void SpreadSalts(Mat img, int num) {
	srand((int)time(NULL)); //�����ų ������ �������� ������ ���� �����Լ��� �ۼ�
	
	int sumRed = 0;
	int sumGreen = 0;
	int sumBlue = 0;

	for (int n = 0; n < num; n++) { //�Է��� ���� ������ŭ �ݺ����� ����
		int x = rand() % img.cols; //img.cols�� �̹����� �� ������ ����
		int y = rand() % img.rows; //img.rows�� �̹����� ���� ������ ����
		int i = rand() % 3; // r,g,b�� �������� �����ϱ� ���� 3�� �������� ����

		 if(img.channels() == 3) {
			img.at<Vec3b>(y, x)[i] = 255; 
			//i�� 0�� ��� Blue ä�� ����
			//i�� 1�� ��� Green ä�� ����
			//i�� 2�� ��� Red ä�� ���� 
		}
		
		 if (i == 0) { //Blue �� �� 1�� ����
			 sumBlue++;
		 }
		 else if (i == 1) {//Green�� �� 1�� ����
			 sumGreen++;
		 }
		 else if (i == 2) {//Red�� �� 1�� ����
			 sumRed++;
		 }
	}
	//ī��Ʈ �� �� ���
	cout << "B : " << sumBlue << endl; 
	cout << "G : " << sumGreen << endl;
	cout << "R : " << sumRed << endl;

}


int main() {
	Mat src_img = imread("C:\\DIP_Pic\\img1.jpg",1); // �̹��� �б�
	// int flags = IMREAD_COLOR �Ǵ� int flags = 1 -> �÷� �������� ����
	// int flags = IMREAD_GRAYSCALE �Ǵ� int flags = 0 -> ��� �������� ����
	// int flags = IMREAD_UNCHANGED �Ǵ� int flags = -1 -> ���� ������ ���Ĵ�� ����
	SpreadSalts(src_img, 100000);
	imshow("Test window", src_img); // �̹��� ���
	waitKey(0); // Ű �Է� ���(0: Ű�� �Էµ� �� ���� ���α׷� ����)
	destroyWindow("Test window"); // �̹��� ���â ����
	imwrite("img1.jpg", src_img); // �̹��� ����
	return 0;
}



// HW Img2

#include <iostream>
#include "opencv2/core/core.hpp" // Mat class�� ���� data structure �� ��� ��ƾ�� �����ϴ� ���
#include "opencv2/highgui/highgui.hpp" // GUI�� ���õ� ��Ҹ� �����ϴ� ���(imshow ��)
#include "opencv2/imgproc/imgproc.hpp" // ���� �̹��� ó�� �Լ��� �����ϴ� ���

using namespace cv;
using namespace std;

Mat darker(Mat img) {
	Mat img2;
	for (int m = 0; m < img.cols; m++) {
		for (int n = 0; n < img.rows; n++) {
			img2 = img.at<uchar>(m,n) + m;
			return img2;
		}
	}
}


int main() {
	Mat src_img = imread("C:\\DIP_Pic\\img2.jpg", 0); // �̹��� �б�
	// int flags = IMREAD_COLOR �Ǵ� int flags = 1 -> �÷� �������� ����
	// int flags = IMREAD_GRAYSCALE �Ǵ� int flags = 0 -> ��� �������� ����
	// int flags = IMREAD_UNCHANGED �Ǵ� int flags = -1 -> ���� ������ ���Ĵ�� ����
	//Mat black(src_img.size(), CV_8UC3, Scalar(0));
	darker(src_img);
	imshow("Test window", src_img); // �̹��� ���
	waitKey(0); // Ű �Է� ���(0: Ű�� �Էµ� �� ���� ���α׷� ����)
	destroyWindow("Test window"); // �̹��� ���â ����
	imwrite("img2.jpg", src_img); // �̹��� ����
	return 0;
}

