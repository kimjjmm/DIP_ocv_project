
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
void readImagesAndTimes(vector<Mat>& images, vector<float>& times) {
    int numImages = 4; //�̹����� ����
    static const float timeArray[] = { 1 / 30.0f, 0.25f, 2.5f, 15.0f }; //���� �ð�
    times.assign(timeArray, timeArray + numImages);//times���Ϳ� ����ð� �迭�� �Ҵ�
    static const char* filenames[] = { "C:\\DIP_Pic\\14\\1.jpg", "C:\\DIP_Pic\\14\\2.jpg",
        "C:\\DIP_Pic\\14\\3.jpg","C:\\DIP_Pic\\14\\4.jpg"  };
    for (int i = 0; i < numImages; i++) {
        Mat im = imread(filenames[i]); //�̹��� �о����
        images.push_back(im); //images���Ϳ� �̹��� �߰�
    }
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


    return 0;
}

