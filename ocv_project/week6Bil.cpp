#include <iostream>
#include "opencv2/core/core.hpp" // Mat class�� ���� data structure �� ��� ��ƾ�� �����ϴ� ���
#include "opencv2/highgui/highgui.hpp" // GUI�� ���õ� ��Ҹ� �����ϴ� ���(imshow ��)
#include "opencv2/imgproc/imgproc.hpp" // ���� �̹��� ó�� �Լ��� �����ϴ� ���

using namespace cv;
using namespace std;

double gaussian(float x, double sigma) { //����þ� �Լ�
	return exp(-(pow(x, 2)) / (2 * pow(sigma, 2))) / (2 * CV_PI * pow(sigma, 2));
}
float distance(int x, int y, int i, int j) { //(x,y)�� (i,j)������ �Ÿ� ���
	return float(sqrt(pow(x - i, 2) + pow(y - j, 2)));
}

void bilateral(const Mat& src_img, Mat& dst_img, int c, int r, int diameter, 
	double sig_r, double sig_s) {
	int radius = diameter / 2; //�������� �ݰ�
	double gr, gs, wei;
	double tmp = 0.;
	double sum = 0.;

	//Ŀ�� �ε���
	for (int kc = -radius; kc <= radius; kc++) {
		for (int kr = -radius; kr <= radius; kr++) {
			
			//range calc
			gr = gaussian(distance(c, r, c + kc, r + kr), sig_r);
			//spatial calc;
			gs = gaussian(src_img.at<uchar>(c, r) - src_img.at<uchar>(c + kc, r + kr), sig_s);
			
			wei = gr * gs; //����ġ
			tmp += src_img.at<uchar>(c + kr, r + kr) * wei; //����ġ�� �ȼ��� ���ϱ�
			sum += wei; //����ġ �� ���ϱ�
		}
	}
	dst_img.at<double>(c, r) = tmp / sum; //����ȭ
}


void myBilateral(const Mat& src_img, Mat& dst_img, int diameter, double sig_r, double sig_s)
{
	dst_img = Mat::zeros(src_img.size(), CV_8UC1); //��� �̹��� �ʱ�ȭ

	Mat guide_img = Mat::zeros(src_img.size(), CV_64F); //���̵� �̹��� �ʱ�ȭ
	int wh = src_img.cols; int hg = src_img.rows;
	int radius = diameter / 2;

	// <�ȼ� �ε��� (�����ڸ� ����) >
	for (int c = radius + 1; c < hg - radius; c++) {
		for (int r = radius + 1; r < wh - radius; r++) {
			bilateral(src_img, guide_img, c, r, diameter, sig_r, sig_s);
			//ȭ�Һ� Bilateral ��� ����
		}

	}
	guide_img.convertTo(dst_img, CV_8UC1); // Mat type��ȯ
}

void doBilateralEx() {
	cout << "--- doBilateralEx() --- \n" << endl;

	//<�Է�>
	Mat src_img = imread("C:\\DIP_Pic\\6\\rock.png", 0);
	Mat dst_img;
	if (!src_img.data) printf("No image data \n");

	myBilateral(src_img, dst_img, 5, 25.0, 50.0);

	//<���>
	Mat result_img;
	hconcat(src_img, dst_img, result_img);
	imshow("doBilateralEx()", result_img);
	waitKey(0);

}

int main() {
	// Mat src_img = imread("C:\\DIP_Pic\\6\\salt_pepper.png", 0); // �̹��� �б�
	// int flags = IMREAD_COLOR �Ǵ� int flags = 1 -> �÷� �������� ����
	// int flags = IMREAD_GRAYSCALE �Ǵ� int flags = 0 -> ��� �������� ����
	// int flags = IMREAD_UNCHANGED �Ǵ� int flags = -1 -> ���� ������ ���Ĵ�� ����

	doBilateralEx();

	waitKey(0); // Ű �Է� ���(0: Ű�� �Էµ� �� ���� ���α׷� ����)
	destroyWindow("Test window"); // �̹��� ���â ����
	//imwrite("gear.jpg", doL); // �̹��� ����

	return 0;
}