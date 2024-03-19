
#include <iostream>
#include "opencv2/core/core.hpp" // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더
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


// < 영상, 노출 시간 불러오기 >
void readImagesAndTimes(vector<Mat>& images, vector<float>& times) {
    int numImages = 4; //이미지의 개수
    static const float timeArray[] = { 1 / 30.0f, 0.25f, 2.5f, 15.0f }; //노출 시간
    times.assign(timeArray, timeArray + numImages);//times벡터에 노출시간 배열을 할당
    static const char* filenames[] = { "C:\\DIP_Pic\\14\\1.jpg", "C:\\DIP_Pic\\14\\2.jpg",
        "C:\\DIP_Pic\\14\\3.jpg","C:\\DIP_Pic\\14\\4.jpg"  };
    for (int i = 0; i < numImages; i++) {
        Mat im = imread(filenames[i]); //이미지 읽어오기
        images.push_back(im); //images벡터에 이미지 추가
    }
}

int main()
{
    // < 영상, 노출 시간 불러오기 >
    cout << "Reading images and exposure times ... " << endl;
    vector<Mat> images;
    vector<float> times;
    readImagesAndTimes(images, times); //이미지와 노출 시간 불러오기
    cout << "finished" << endl;

    //<영상 정렬>
    cout << "Aligning images ..." << endl;
    Ptr<AlignMTB> alignMTB = createAlignMTB(); //객체 생성
    alignMTB->process(images, images);//이미지 정렬해서 images벡터에 저장

    //CRF 복원
    cout << "Calculating Camera Response Function ..." << endl;
    Mat responseDebevec;
    Ptr<CalibrateDebevec> calibrateDebevec = createCalibrateDebevec();//객체 생성
    calibrateDebevec->process(images, responseDebevec, times);//CRF계산
    cout << "----- CRF ----- " << endl;
    cout << responseDebevec << endl;


    //24bit 표현 범위로 이미지 병합
    cout << "Merging image into HDR image..." << endl;
    Mat hdrDebevec;
    Ptr<MergeDebevec> mergeDebevec = createMergeDebevec(); //객체 생성
    mergeDebevec->process(images, hdrDebevec, times, responseDebevec);//이미지를 HDR이미지로 병합
    imwrite("hdrDebevec.hdr", hdrDebevec);
    cout << "saved hdrDebevec.hdr" << endl;


    //Drago톤맵
    cout << "Tonemaping using Drago's method...";
    Mat IdrDrago;
    Ptr<TonemapDrago> tonemapDrago = createTonemapDrago(1.0f, 0.7f, 0.85f); //객체 생성
    tonemapDrago->process(hdrDebevec, IdrDrago); //톤맵처리
    IdrDrago = 3 * IdrDrago;
    imwrite("Idr-Drago.jpg", IdrDrago * 255);
    cout << "saved Idr-Drago.jpg" << endl;

    //Reinhard 톤맵
    cout << "Tonemaping using Reinhard's method...";
    Mat IdrReinhard;
    Ptr<TonemapReinhard> tonemapReinhard = createTonemapReinhard(1.5f, 0, 0, 0); //객체 생성
    tonemapReinhard->process(hdrDebevec, IdrReinhard); //톤맵처리
    imwrite("Idr-Reinhard.jpg", IdrReinhard * 255);
    cout << "saved Idr-Reinhard.jpg" << endl;

    //Mantiuk톤맵
    cout << "Tonemaping using Mantiuk's method...";
    Mat IdrMantiuk;
    Ptr<TonemapMantiuk> tonemapMantiuk = createTonemapMantiuk(2.2f, 0.85f, 1.2f); //객체 생성
    tonemapMantiuk->process(hdrDebevec, IdrMantiuk); //톤맵처리
    IdrMantiuk = 3 * IdrMantiuk;
    imwrite("Idr-Mantiuk.jpg", IdrMantiuk * 255);
    cout << "saved Idr-Mantiuk.jpg" << endl;


    return 0;
}

