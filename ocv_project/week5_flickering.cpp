//flickering 12205102������

#include <iostream>
#include "opencv2/core/core.hpp" // Mat class�� ���� data structure �� ��� ��ƾ�� �����ϴ� ���
#include "opencv2/highgui/highgui.hpp" // GUI�� ���õ� ��Ҹ� �����ϴ� ���(imshow ��)
#include "opencv2/imgproc/imgproc.hpp" // ���� �̹��� ó�� �Լ��� �����ϴ� ���

using namespace cv;
using namespace std;

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


Mat setComplex(Mat magImg, Mat phaImg) {
	exp(magImg, magImg);
	magImg -= Scalar::all(1); //magnitude ����� �ݴ�� ����

	Mat planes[2];
	polarToCart(magImg, phaImg, planes[0], planes[1]); //�� ��ǥ�� -> ������ǥ��

	Mat complexImg;
	merge(planes, 2, complexImg);
	//�Ǽ���, ����� ��ü
	return complexImg;
}

Mat doIdft(Mat complexImg) {//Ǫ���� ����ȯ
	Mat idftcvt;
	idft(complexImg, idftcvt);
	//IDFT�� �̿��� ���� ���� ���

	Mat planes[2];
	split(idftcvt, planes);

	Mat dstImg;
	magnitude(planes[0], planes[1], dstImg);
	normalize(dstImg, dstImg, 255, 0, NORM_MINMAX);
	dstImg.convertTo(dstImg, CV_8UC1);
	// �Ϲ� ������ type�� ǥ�������� ��ȯ

	return dstImg;
}
Mat padding(Mat img) {
	int dftRows = getOptimalDFTSize(img.rows);
	int dftCols = getOptimalDFTSize(img.cols);

	Mat padded;
	copyMakeBorder(img, padded, 0, dftRows - img.rows, 0, dftCols - img.cols, BORDER_CONSTANT, Scalar::all(0));
	return padded;
}

Mat doFlic(Mat srcImg) {

	//<DFT>
	Mat padImg = padding(srcImg);
	Mat complexImg = doDft(padImg);
	Mat centerComplexImg = centralize(complexImg);
	Mat magImg = getMagnitude(centerComplexImg);
	Mat phaImg = getPhase(centerComplexImg);

	//flickering ���� ����
	double minVal, maxVal;
	Point minLoc, maxLoc;
	minMaxLoc(magImg, &minVal, &maxVal, &minLoc, &maxLoc);//�ִ밪 �ּҰ� �� ����ȭ
	normalize(magImg, magImg, 0, 1, NORM_MINMAX);

	Mat maskImg = Mat::ones(magImg.size(), CV_32F); //ones�� ��� ���Ұ� 1�� ��� ��ȯ�س���
	line(maskImg, Point(maskImg.cols /2, 0), Point(maskImg.cols / 2, maskImg.rows), Scalar::all(0), 25, 8, 0); //maskimg�� ���� ����
	circle(maskImg, Point(maskImg.cols / 2, maskImg.rows / 2),0.5, Scalar::all(1), -1, -1, 0); 
	
	//line(maskImg, Point(maskImg.cols / 2, maskImg.rows *6/ 11), Point(maskImg.cols / 2, maskImg.rows), Scalar::all(0), 30, 4, 0);
	//line(maskImg, Point(maskImg.cols / 2, maskImg.rows * 10/ 250), Point(maskImg.cols / 2, maskImg.rows * 11/ 250), Scalar::all(0), 20, 4, 0);
	//line(maskImg, Point(maskImg.cols / 2, maskImg.rows * 8 / 250), Point(maskImg.cols / 2, maskImg.rows * 9 / 250), Scalar::all(0), 20, 4, 0);
	//line(maskImg, Point(maskImg.cols / 2, maskImg.rows * 2/ 5), Point(maskImg.cols / 2, maskImg.rows*3/5), Scalar::all(0), 1, 8, 0);
	//line(maskImg, Point(maskImg.cols *5/11, maskImg.rows / 2), Point(maskImg.cols *6/11, maskImg.rows /2), Scalar::all(0), 1, 8, 0);

	Mat magImg2;
	multiply(magImg, maskImg, magImg2);
	imshow("remove", magImg2);	
	//����ȯ �� flickering ������ ������ magnitude �̹��� ���

	//<IDFT>
	normalize(magImg2, magImg2, (float)minVal, (float)maxVal, NORM_MINMAX);
	Mat complexImg2 = setComplex(magImg2, phaImg);
	Mat dstImg = doIdft(complexImg2);


	return myNormalize(dstImg);
}

int main() {
	Mat src_img = imread("C:\\DIP_Pic\\5\\img3.JPG", 0); // �̹��� �б�
	// int flags = IMREAD_COLOR �Ǵ� int flags = 1 -> �÷� �������� ����
	// int flags = IMREAD_GRAYSCALE �Ǵ� int flags = 0 -> ��� �������� ����
	// int flags = IMREAD_UNCHANGED �Ǵ� int flags = -1 -> ���� ������ ���Ĵ�� ����

	//dft. �����̹����� ���� magnitude ����
	Mat padd = padding(src_img);
	Mat freq = doDft(padd);
	Mat cent = centralize(freq);
	Mat mag = getMagnitude(cent);
	Mat nor = myNormalize(mag);
	
	Mat flicimg = doFlic(src_img);//flickering ���� ����

	imshow("origin", src_img); //�������� ���
	imshow("originmag", nor); //���������� magnitude���� ���

	imshow("flickimg", flicimg);

	waitKey(0); // Ű �Է� ���(0: Ű�� �Էµ� �� ���� ���α׷� ����)
	destroyWindow("Test window"); // �̹��� ���â ����
	//imwrite("gear.jpg", doL); // �̹��� ����

	return 0;
}