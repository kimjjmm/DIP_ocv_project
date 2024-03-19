#include <opencv2/opencv.hpp>
#include <iostream>
#include "opencv2/core/core.hpp" // Mat class�� ���� data structure �� ��� ��ƾ�� �����ϴ� ���
#include "opencv2/highgui/highgui.hpp" // GUI�� ���õ� ��Ҹ� �����ϴ� ���(imshow ��)
#include "opencv2/imgproc/imgproc.hpp" // ���� �̹��� ó�� �Լ��� �����ϴ� ���
using namespace cv;
using namespace std;

void createClustersInfo(Mat imgInput, int n_cluster,
    vector<Scalar>& clustersCenters, vector<vector<Point>>& ptInclusters) {
    RNG random(getTickCount()); //OpenCV���� ������ ���� �����ϴ� �Լ�

    for (int k = 0; k < n_cluster; k++) {//������ ���
        //<������ ��ǥ ȹ��>
        Point centerKPoint;
        centerKPoint.x = random.uniform(0, imgInput.cols);
        centerKPoint.y = random.uniform(0, imgInput.rows);
        Scalar centerPixel = imgInput.at<Vec3b>(centerKPoint.y, centerKPoint.x);

        //<������ ��ǥ�� ȭ�Ұ����� �������߾Ӱ� ����>
        Scalar centerK(centerPixel.val[0], centerPixel.val[1], centerPixel.val[2]);
        clustersCenters.push_back(centerK);

        vector<Point>ptInclustersK;
        ptInclusters.push_back(ptInclustersK);
    }
}
Mat applyFinalClusterToImage(Mat src_img, int n_cluster,
    vector<vector<Point>>ptInClusters,
    vector<Scalar>clustersCenters) {

    Mat dst_img(src_img.size(), src_img.type());

    for (int k = 0; k < n_cluster; k++) {//��� ������ ���� ����
        vector<Point>ptInCluster = ptInClusters[k]; //������ ��ǥ��
        for (int j = 0; j < ptInCluster.size(); j++) {
            //������ ��ǥ ��ġ�� �ִ� ȭ�� ���� �ش� ���� �߾Ӱ����� ��ü
            dst_img.at<Vec3b>(ptInCluster[j])[0] = clustersCenters[k].val[0];
            dst_img.at<Vec3b>(ptInCluster[j])[1] = clustersCenters[k].val[1];
            dst_img.at<Vec3b>(ptInCluster[j])[2] = clustersCenters[k].val[2];
        }
    }
    return dst_img;
}
Mat applyFinalClusterToImage(Mat src_img, int n_cluster,
    vector<vector<Point>>ptInClusters,
    vector < vector<Scalar>>clustersCenters) {
    Mat dst_img(src_img.size(), src_img.type());

    srand(time(NULL));
    for (int k = 0; k < n_cluster; k++) {
        vector<Point>ptInCluster = ptInClusters[k];
        Scalar randomColor(rand() % 255, rand() % 255, rand() % 255);
        for (int i = 0; i < ptInCluster.size(); i++) {
            Scalar pixelColor = dst_img.at<Vec3b>(ptInCluster[i]);
            pixelColor = randomColor;

            dst_img.at<Vec3b>(ptInCluster[i])[0] = pixelColor.val[0];
            dst_img.at<Vec3b>(ptInCluster[i])[1] = pixelColor.val[1];
            dst_img.at<Vec3b>(ptInCluster[i])[2] = pixelColor.val[2];
        }
    }
    return dst_img;
}
double computeColorDistance(Scalar pixel, Scalar clusterPixel) {
    double diffBlue = pixel.val[0] - clusterPixel[0];
    double diffGreen = pixel.val[1] - clusterPixel[1];
    double diffRed = pixel.val[2] - clusterPixel[2];

    double distance = sqrt(pow(diffBlue, 2) + pow(diffGreen, 2) + pow(diffRed, 2));
    //Euclidian distance
    return distance;
}
double adjustClusterCenters(Mat src_img, int n_cluster,
    vector<Scalar>& clustersCenters, vector<vector<Point>>ptInClusters,
    double& oldCenter, double newCenter) {

    double diffChange;

    for (int k = 0; k < n_cluster; k++) //������ ���
    {
        vector<Point>ptInCluster = ptInClusters[k];
        double newBlue = 0;
        double newGreen = 0;
        double newRed = 0;

        //<��հ� ���>
        for (int i = 0; i < ptInCluster.size(); i++) {
            Scalar pixel = src_img.at<Vec3b>(ptInCluster[i].y, ptInCluster[i].x);
            newBlue += pixel.val[0];
            newGreen += pixel.val[1];
            newRed += pixel.val[2];
        }
        newBlue /= ptInCluster.size();
        newGreen /= ptInCluster.size();
        newRed /= ptInCluster.size();

        //<����� ��հ����� ���� �߾Ӱ� ��ü>
        Scalar newPixel(newBlue, newGreen, newRed);
        newCenter += computeColorDistance(newPixel, clustersCenters[k]);
        //��� ������ ���� ��հ��� ���� ���
        clustersCenters[k] = newPixel;
    }
    newCenter /= n_cluster;
    diffChange = abs(oldCenter - newCenter);
    //��� ������ ���� ��հ� ��ȭ�� ���
    oldCenter = newCenter;

    return diffChange;
}
void findAssociatedCluster(Mat imgInput, int n_cluster,
    vector<Scalar>clustersCenters, vector<vector<Point>>& ptInClusters) {
    for (int r = 0; r < imgInput.rows; r++) {
        for (int c = 0; c < imgInput.cols; c++) {
            double minDistance = INFINITY;
            int closestClusterIndex = 0;
            Scalar pixel = imgInput.at<Vec3b>(r, c);

            for (int k = 0; k < n_cluster; k++) { //������ ���
                //<�� ���� �߾Ӱ����� ���̸� ���>
                Scalar clusterPixel = clustersCenters[k];
                double distance = computeColorDistance(pixel, clusterPixel);

                //<���̰� ���� ���� �������� ��ǥ�� ������ �Ǻ�>
                if (distance < minDistance) {
                    minDistance = distance;
                    closestClusterIndex = k;
                }
            }
            //<��ǥ ����>
            ptInClusters[closestClusterIndex].push_back(Point(c, r));
        }
    }
}
Mat MyKMeans(Mat src_img, int n_cluster) {
    vector<Scalar>clustersCenters; //���� �߾Ӱ� ����
    vector<vector<Point>>ptInClusters; //������ ��ǥ ����
    double threshold = 0.001;
    double oldCenter = INFINITY;
    double newCenter = 0;
    double diffChange = oldCenter - newCenter; //���� ������ ��ȭ��

    //<�ʱ⼳��>
    //���� �߾Ӱ��� �������� �Ҵ� �� ������ ��ǥ���� ������ ���� �Ҵ�
    createClustersInfo(src_img, n_cluster, clustersCenters, ptInClusters);

    //<�߾Ӱ� ���� �� ȭ�Һ� ���� �Ǻ�>
    //�ݺ����� ������� ���� �߾Ӱ� ����
    //������ �Ӱ谪 ���� ���� ������ ��ȭ�� ���� ������ �ݺ�
    while (diffChange > threshold) {
        //�ʱ�ȭ
        newCenter = 0;
        for (int k = 0; k < n_cluster; k++) { ptInClusters[k].clear(); }

        //<������ ���� �߾Ӱ��� �������� ���� Ž��>
        findAssociatedCluster(src_img, n_cluster, clustersCenters, ptInClusters);

        //<���� �߾Ӱ� ����>
        diffChange = adjustClusterCenters(src_img, n_cluster, clustersCenters, ptInClusters,
            oldCenter, newCenter);
    }

    //<���� �߾Ӱ����θ� �̷���� ���� ����>
    Mat dst_img = applyFinalClusterToImage(src_img, n_cluster, ptInClusters, clustersCenters);

    return dst_img;
}


Mat MyBgr2Hsv(Mat src_img) {
    //BGR�� �������� ǥ���� �̹����� HSV�� ��ȯ
    double b, g, r, h, s, v;
    Mat dst_img(src_img.size(), src_img.type());
    //�Է��̹����� ���� ũ��� Ÿ���� ������ ����̹��� ����
    for (int y = 0; y < src_img.rows; y++) {
        for (int x = 0; x < src_img.cols; x++) {
            b = (double)src_img.at<Vec3b>(y, x)[0];
            g = (double)src_img.at<Vec3b>(y, x)[1];
            r = (double)src_img.at<Vec3b>(y, x)[2];
            //��� �ȼ��� ���� �ݺ��ϰ� �ȼ��� BGR���� b,g,r������ ����


            // BGR ���� HSV ������ ��ȯ
            double max_val = max(max(b, g), r);
            double min_val = min(min(b, g), r);
            double delta = max_val - min_val;
            //delta: �ִ밪 - �ּҰ� ����

            //Hue ���
            if (max_val == r) {
                h = 60 * (0 + (g - b) / (max_val - min_val));
            }
            if (max_val == g) {
                h = 60 * (2 + (b - r) / (max_val - min_val));
            }
            if (max_val == b) {
                h = 60 * (4 + (r - g) / (max_val - min_val));
            }
            if (h < 0) {
                h = h + 360; // hue�� �����̸�, h +30���� hue�� �ʱ�ȭ
            }

            //Saturation ���
            if (delta == 0) { //max-min = 0
                s = 0;
            }
            else
                s = (max_val - min_val) / max_val;

            //Value���
            v = max_val; //value���� ��⸦ ��Ÿ���� �����μ�, �ȼ� �� ���� ū ���� max_val�� value�� ��

           // Mat hsv_img;
            dst_img.at<Vec3b>(y, x)[0] = (uchar)(h / 2);
            dst_img.at<Vec3b>(y, x)[1] = (uchar)(s * 255);
            dst_img.at<Vec3b>(y, x)[2] = (uchar)v;
        }
    }

    return dst_img;
}



Mat MyinRange(Mat src_img, Scalar lower, Scalar upper) {

    Mat dst_img(src_img.rows, src_img.cols, CV_8UC1);
    for (int y = 0; y < src_img.rows; y++) {
        for (int x = 0; x < src_img.cols; x++) {
            double b = (double)src_img.at<Vec3b>(y, x)[0];
            double g = (double)src_img.at<Vec3b>(y, x)[1];
            double r = (double)src_img.at<Vec3b>(y, x)[2];
            //��� �ȼ��� ���� �ݺ��ϰ� ���� bgr�� ���� ����

            //bgr�� ������
            bool in_range = (lower[0] <= b && b <= upper[0]) &&
                (lower[1] <= g && g <= upper[1]) &&
                (lower[2] <= r && r <= upper[2]);
            if (in_range) {//lower-upper ���� ���� �ִ� �ȼ��� ���
                dst_img.at<uchar>(y, x) = 255;
            }
            else {//���� �ܿ� �ִ� �ȼ��� ������
                dst_img.at<uchar>(y, x) = 0;
            }

        }
    }
    return dst_img;
}


int main() {
    Mat src_img = imread("C:\\DIP_Pic\\7\\apple3.jpg", 1); // �̹��� �б�

    Mat myKMeansResult = MyKMeans(src_img, 4); // ���� ������ �Լ� ���

    imshow("myKMeansResult", myKMeansResult);

  //  waitKey(0); // Ű �Է� ���(0: Ű�� �Էµ� �� ���� ���α׷� ����)
   // destroyWindow("myKMeansResult"); // �̹��� ���â ����


   //  
    //����
    Scalar lower_red = Scalar(0, 50, 100);
    Scalar upper_red = Scalar(10, 255, 255);
    //�Ķ�
    Scalar lower_blue = Scalar(100, 50, 100);
    Scalar upper_blue = Scalar(120, 255, 255);
    //�ʷ�
    Scalar lower_green = Scalar(30, 30, 50);
    Scalar upper_green = Scalar(50, 255, 255);



   //Mat src_img = imread("C:\\DIP_Pic\\7\\apple3.jpg", 1); // �̹��� �б�
     //hsv�ٲٱ�
   Mat hsv_img = MyBgr2Hsv(myKMeansResult);

    //������ ����ũ �����
    Mat mask_red = MyinRange(hsv_img, lower_red, upper_red);
    Mat mask_blue = MyinRange(hsv_img, lower_blue, upper_blue);
    Mat mask_green = MyinRange(hsv_img, lower_green, upper_green);

    // �� �������� ���� ���� �ȼ��� ���� ���� ���
    int red_pixels = countNonZero(mask_red);
    int blue_pixels = countNonZero(mask_blue);
    int green_pixels = countNonZero(mask_green);

    Mat final;

    if (red_pixels > blue_pixels && red_pixels > green_pixels)
    {
        cout << "The image is red" << endl;
        bitwise_and(src_img, src_img, final, mask_red);
    }
    else if (blue_pixels > red_pixels && blue_pixels > green_pixels)
    {
        cout << "The image is blue" << endl;
        bitwise_and(src_img, src_img, final, mask_blue);
    }
    else if (green_pixels > red_pixels && green_pixels > blue_pixels)
    {
        cout << "The image is green" << endl;
        bitwise_and(src_img, src_img, final, mask_green);
    }
    else
    {
        cout << "The image contains multiple colors" << endl;
    }

    imshow("final", final);

    waitKey(0); // Ű �Է� ���(0: Ű�� �Էµ� �� ���� ���α׷� ����)

    return 0;
}