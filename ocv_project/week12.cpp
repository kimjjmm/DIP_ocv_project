#include <opencv2/opencv.hpp>
#include <iostream>
#include "opencv2/core/core.hpp" // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더

using namespace cv;
using namespace std;

void cvFlip() {
	Mat src = imread("C:\\DIP_Pic\\12\\Lenna.png", 1);
	Mat dst_x, dst_y, dst_xy;

	flip(src, dst_x, 0);
	flip(src, dst_y, 1);
	flip(src, dst_xy, -1);

	imwrite("nonflip.jpg", src);
	imwrite("xflip.jpg", dst_x);
	imwrite("yflip.jpg", dst_y);
	imwrite("xyflip.jpg", dst_xy);

	imshow("nonflip", src);
	imshow("xflip", dst_x);
	imshow("yflip", dst_y);
	imshow("xyflip", dst_xy);

	waitKey(0);
	destroyAllWindows();
}

void cvRotation() {
	Mat src = imread("C:\\DIP_Pic\\12\\Lenna.png", 1);
	Mat dst, matrix;

	Point center = Point(src.cols / 2, src.rows / 2);
	matrix = getRotationMatrix2D(center, 45.0, 1.0);
	warpAffine(src, dst, matrix, src.size());

	imwrite("nonrot.jpg", src);
	imwrite("rot.jpg", dst);

	imshow("nonrot", src);
	imshow("cvrot", dst);
	waitKey(0);

	destroyAllWindows();
}

void cvAffine() {
	Mat src = imread("C:\\DIP_Pic\\12\\Lenna.png", 1);
	Mat dst, matrix;

	Point2f srcTri[3];
	srcTri[0] = Point2f(0.f, 0.f);
	srcTri[1] = Point2f(src.cols - 1.f, 0.f);
	srcTri[2] = Point2f(0.f, src.rows - 1.f);

	Point2f dstTri[3];
	dstTri[0] = Point2f(0.f, src.rows * 0.33f);
	dstTri[1] = Point2f(src.cols * 0.85f, src.rows * 0.25f);
	dstTri[2] = Point2f(src.cols * 0.15f, src.rows * 0.7f);

	matrix = getAffineTransform(srcTri, dstTri);
	warpAffine(src, dst, matrix, src.size());

	imwrite("nonaff.jpg", src);
	imwrite("aff.jpg", dst);

	imshow("nonaff", src);
	imshow("aff", dst);
	waitKey(0);

	destroyAllWindows();
}

void cvPerspective()
{
	Mat src = imread("C:\\DIP_Pic\\12\\Lenna.png", 1);
	Mat dst, matrix;

	Point2f srcQuad[4];
	srcQuad[0] = Point2f(0.f, 0.f);
	srcQuad[1] = Point2f(src.cols - 1.f, 0.f);
	srcQuad[2] = Point2f(0.f, src.rows - 1.f);
	srcQuad[3] = Point2f(src.cols - 1.f, src.rows - 1.f);

	Point2f dstQuad[4];
	dstQuad[0] = Point2f(0.f, src.rows * 0.33f);
	dstQuad[1] = Point2f(src.cols * 0.85f, src.rows * 0.25f);
	dstQuad[2] = Point2f(src.cols * 0.15f, src.rows * 0.7f);
	dstQuad[3] = Point2f(src.cols * 0.85f, src.rows - 0.7f);

	matrix = getPerspectiveTransform(srcQuad, dstQuad);
	warpPerspective(src, dst, matrix, src.size());

	imwrite("nonper.jpg", src);
	imwrite("per.jpg", dst);

	imshow("nonper", src);
	imshow("per", dst);
	waitKey(0);

	destroyAllWindows();
}

Mat myTransMat() {
	Mat matrix1 = (Mat_<double>(3, 3) <<
		1, tan(45 * CV_PI / 180), 0,
		0, 1, 0,
		0, 0, 1);
	Mat matrix2 = (Mat_<double>(3, 3) <<
		1, 0, -256,
		0, 1, 0,
		0, 0, 1);
	Mat matrix3 = (Mat_<double>(3, 3) <<
		0.5, 0, 0,
		0, 0.5, 0,
		0, 0, 1);
	return matrix3 * matrix2 * matrix1;
}
void cvPerspective2() {
	Mat src = imread("C:\\DIP_Pic\\12\\Lenna.png", 1);
	Mat dst, matrix;

	matrix = myTransMat();
	warpPerspective(src, dst, matrix, src.size());

	imwrite("nonper.jpg", src);
	imwrite("per.jpg", dst);

	imshow("nonper", src);
	imshow("per", dst);
	waitKey(0);

	destroyAllWindows();
}



int main() {
	//cvFlip();
	cvRotation();
	//cvAffine();
	//cvPerspective();
	//cvPerspective2();
}