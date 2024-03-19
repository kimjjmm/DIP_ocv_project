#include <opencv2/opencv.hpp>
#include <iostream>
#include "opencv2/core/core.hpp" // Mat class�� ���� data structure �� ��� ��ƾ�� �����ϴ� ���
#include "opencv2/highgui/highgui.hpp" // GUI�� ���õ� ��Ҹ� �����ϴ� ���(imshow ��)
#include "opencv2/imgproc/imgproc.hpp" // ���� �̹��� ó�� �Լ��� �����ϴ� ���

#include <opencv2/objdetect/objdetect.hpp> //��ü ���⿡ ���Ǵ� �Լ� ���� ���
#include <opencv2/features2d.hpp>//�̹��� Ư¡�� ���� �� ��Ī �Լ� ���� ���

using namespace cv;
using namespace std;

void cvHarrisConer() {
	Mat img = imread("C:\\DIP_Pic\\11\\square.png"); //�̹��� �ҷ�����
	//�̹��� ���� ��� ����
	if (img.empty()) {
		cout << "Empty imged!\n";
		exit(-1);
	}
	//�̹��� ũ�� ����
	resize(img, img, Size(500, 500), 0, 0, INTER_CUBIC);

	//�׷��� ������ �̹��� ����
	Mat gray;
	cvtColor(img, gray, CV_BGR2GRAY);

	//�ظ����ڳ� ���� ����
	Mat harr;
	cornerHarris(gray, harr, 2, 3, 0.05, BORDER_DEFAULT);
	
	//����̹��� normalize
	normalize(harr, harr, 0,255,NORM_MINMAX,CV_32FC1,Mat());
	Mat harr_abs;
	convertScaleAbs(harr, harr_abs);

	//�Ӱ谪 �������� �ڳʿ� �� �׸���
	int thresh = 125;
	Mat result = img.clone();
	for (int y = 0; y < harr.rows; y += 1) {
		for (int x = 0; x < harr.cols; x += 1) {
			if ((int)harr.at<float>(y, x) > thresh)
				circle(result, Point(x, y), 30, Scalar(0, 0, 0), 0, 4, 0);
		}
	}

	//�̹��� ���
	imshow("Souce image", img); //�Է��̹���
	imshow("Harris image", harr_abs); //�ظ����ڳ� �ð�ȭ ���
	imshow("Target imag", result); //������ ���

	//������ȯ�� ���� �̹��� ����
	Mat img_hough;
	result.copyTo(img_hough);

	//�׷��̽����Ϸ� ��ȯ
	Mat gray_result;
	cvtColor(img_hough, gray_result, CV_BGR2GRAY);

	//���� �� ���� ����
	vector<Vec3f>circles;
	HoughCircles(gray_result, circles, HOUGH_GRADIENT, 1, 100, 50, 25, 0, 0);
	for (size_t i = 0; i < circles.size(); i++) {
		Vec3i c = circles[i];
		Point center(c[0], c[1]);
		int radius = c[2];

		//����� �� �׸���
		circle(gray_result, center, radius, Scalar(0, 255, 0), 2);
		circle(gray_result, center, 2, Scalar(0, 0, 255), 3);
	}
	//�� ���� ���
	cout <<  circles.size()<<"����" << endl;

	//������ �̹��� ���
	imshow("img_hough", gray_result);

	waitKey(0); ///������ ���� ���
	destroyAllWindows(); //��������� ����

}

int main() {
	cvHarrisConer();
	//myHarrisConer();
}