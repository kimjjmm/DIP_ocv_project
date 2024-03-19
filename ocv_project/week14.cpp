
#include <iostream>
#include "opencv2/core/core.hpp" // Mat class�� ���� data structure �� ��� ��ƾ�� �����ϴ� ���
#include "opencv2/highgui/highgui.hpp" // GUI�� ���õ� ��Ҹ� �����ϴ� ���(imshow ��)
#include "opencv2/imgproc/imgproc.hpp" // ���� �̹��� ó�� �Լ��� �����ϴ� ���
#include <vector>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/stitching.hpp.>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/photo.hpp>
#include <opencv2/imgcodecs.hpp>



using namespace cv;
using namespace std;
using namespace cv::xfeatures2d;


// < ����, ���� �ð� �ҷ����� >
void readImagesAndTimes(vector<Mat>& images, vector<float>& times){
    int numImages = 4; //�̹����� ����
    static const float timeArray[] = { 1 / 30.0f, 0.25f, 2.5f, 15.0f }; //���� �ð�
    times.assign(timeArray, timeArray + numImages); //times���Ϳ� ���� �ð� �迭�� �Ҵ�
    static const char* filenames[] = { "C:\\DIP_pic\\14\\img_0.033.jpg", "C:\\DIP_Pic\\14\\img_0.25.jpg", 
        "C:\\DIP_pic\\14\\img_2.5.jpg", "C:\\DIP_pic\\14\\img_15.jpg"};
    for (int i = 0; i < numImages; i++) {
        Mat im = imread(filenames[i]); //�̹��� �о����
        images.push_back(im); //images���Ϳ� �̹��� �߰�
    }
}

Mat GetHistogram(const Mat& src) {
    Mat srcGray;
    cvtColor(src, srcGray, COLOR_BGR2GRAY);

    Mat histogram;
    const int* channel_numbers = { 0 };
    float channel_range[] = { 0.0, 255.0 };
    const float* channel_ranges = channel_range;
    int number_bins = 256; 

    calcHist(&srcGray, 1, channel_numbers, Mat(), histogram, 1, &number_bins, &channel_ranges);

    int hist_w = 512;
    int hist_h = 400;
    int bin_w = cvRound((double)hist_w / number_bins);

    Mat histImage(hist_h, hist_w, CV_8UC1, Scalar(0));

    normalize(histogram, histogram, 0, histImage.rows, NORM_MINMAX, -1, Mat());


    for (int i = 1; i < number_bins; i++) {
        line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(histogram.at<float>(i - 1))),
            Point(bin_w * i, hist_h - cvRound(histogram.at<float>(i))),
            Scalar(255), 2, LINE_8, 0);
    }

    return histImage;
}


int main()
{
    // < ����, ���� �ð� �ҷ����� >
    cout << "Reading images and exposure times ... " << endl;
    vector<Mat> images;
    vector<float> times;
    readImagesAndTimes(images, times); //�̹����� ���� �ð� �ҷ�����
    cout << "finished" << endl;

    //<���� ����>
    cout << "Aligning images ..." << endl;
    Ptr<AlignMTB> alignMTB = createAlignMTB(); //��ü ����
    alignMTB->process(images, images);//�̹��� �����ؼ� images���Ϳ� ����

    //CRF ����
    cout << "Calculating Camera Response Function ..." << endl;
    Mat responseDebevec;
    Ptr<CalibrateDebevec> calibrateDebevec = createCalibrateDebevec();//��ü ����
    calibrateDebevec->process(images, responseDebevec, times);//CRF���
    cout << "----- CRF ----- " << endl;
    cout << responseDebevec << endl;


    //24bit ǥ�� ������ �̹��� ����
    cout << "Merging image into HDR image..." << endl;
    Mat hdrDebevec;
    Ptr<MergeDebevec> mergeDebevec = createMergeDebevec(); //��ü ����
    mergeDebevec->process(images, hdrDebevec, times, responseDebevec);//�̹����� HDR�̹����� ����
    imwrite("hdrDebevec.hdr", hdrDebevec);
    cout << "saved hdrDebevec.hdr" << endl;


    //Drago���
    cout << "Tonemaping using Drago's method...";
    Mat IdrDrago;
    Ptr<TonemapDrago> tonemapDrago = createTonemapDrago(1.0f, 0.7f, 0.85f); //��ü ����
    tonemapDrago->process(hdrDebevec, IdrDrago); //���ó��
    IdrDrago = 3 * IdrDrago;
    imwrite("Idr-Drago.jpg", IdrDrago * 255);
    cout << "saved Idr-Drago.jpg" << endl;

    //Reinhard ���
    cout << "Tonemaping using Reinhard's method...";
    Mat IdrReinhard;
    Ptr<TonemapReinhard> tonemapReinhard = createTonemapReinhard(1.5f, 0, 0, 0); //��ü ����
    tonemapReinhard->process(hdrDebevec, IdrReinhard); //���ó��
    imwrite("Idr-Reinhard.jpg", IdrReinhard * 255);
    cout << "saved Idr-Reinhard.jpg" << endl;

    //Mantiuk���
    cout << "Tonemaping using Mantiuk's method...";
    Mat IdrMantiuk;
    Ptr<TonemapMantiuk> tonemapMantiuk = createTonemapMantiuk(2.2f, 0.85f, 1.2f); //��ü ����
    tonemapMantiuk->process(hdrDebevec, IdrMantiuk); //���ó��
    IdrMantiuk = 3 * IdrMantiuk;
    imwrite("Idr-Mantiuk.jpg", IdrMantiuk * 255);
    cout << "saved Idr-Mantiuk.jpg" << endl;


    Mat inputImage1 = imread("C:\\DIP_pic\\14\\img_0.033.jpg");
    if (inputImage1.empty()) {
        cout << "Failed to load input image!" << endl;
        return -1;
    }
    Mat inputImage2 = imread("C:\\DIP_pic\\14\\img_0.25.jpg");
    if (inputImage2.empty()) {
        cout << "Failed to load input image!" << endl;
        return -1;
    }
    Mat inputImage3 = imread("C:\\DIP_pic\\14\\img_2.5.jpg");
    if (inputImage3.empty()) {
        cout << "Failed to load input image!" << endl;
        return -1;
    }
    Mat inputImage4 = imread("C:\\DIP_pic\\14\\img_15.jpg");
    if (inputImage4.empty()) {
        cout << "Failed to load input image!" << endl;
        return -1;
    }

    Mat dragoImage = imread("Idr-Drago.jpg");
    if (dragoImage.empty()) {
        cout << "Failed to load result image!" << endl;
        return -1;
    }
    Mat reinhardImage = imread("Idr-Reinhard.jpg");
    if (reinhardImage.empty()) {
        cout << "Failed to load result image!" << endl;
        return -1;
    }
    Mat mantiukImage = imread("Idr-Mantiuk.jpg");
    if (mantiukImage.empty()) {
        cout << "Failed to load result image!" << endl;
        return -1;
    }

    Mat histogramInput1 = GetHistogram(inputImage1);
    imshow("Histogram - Input 0.033", histogramInput1);
    Mat histogramInput2 = GetHistogram(inputImage2);
    imshow("Histogram - Input 0.25", histogramInput2);
    Mat histogramInput3 = GetHistogram(inputImage3);
    imshow("Histogram - Input 2.5", histogramInput3);
    Mat histogramInput4 = GetHistogram(inputImage4);
    imshow("Histogram - Input 15", histogramInput4);

    Mat histogramDrago = GetHistogram(dragoImage);
    imshow("Histogram - dragoImage", histogramDrago);

    Mat histogramReinhard = GetHistogram(reinhardImage);
    imshow("Histogram - reinhardImage", histogramReinhard);

    Mat histogramMantiuk = GetHistogram(mantiukImage);
    imshow("Histogram - mantiukImage", histogramMantiuk);

    waitKey(0);

   return 0;
}

