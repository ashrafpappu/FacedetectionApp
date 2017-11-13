

#ifndef FACEDETECTIONANDROID_OPEN_CV_FACEDETECTOR_H_H
#define FACEDETECTIONANDROID_OPEN_CV_FACEDETECTOR_H_H

#include <opencv2/imgproc.hpp>
#include <vector>
#include <opencv2/objdetect.hpp>
#include <chrono>
#include <android/log.h>
#include <opencv2/imgcodecs.hpp>

#include "TrainAbleDetector.h"
#include "FaceDetectionLogger.h"
#include "FaceDetectionResult.h"
#include <string>
#include <sstream>

using namespace std;
using namespace cv;

enum DetectAngleOrientation{
    right,middle,left
};

class OpenCvFaceDetector :  public TrainAbleDetector {
public:
    OpenCvFaceDetector(){};
    bool loadTrainData(std::string resourceFilePath);
   FaceDetectionResult detectFaces(unsigned char* imageBuf, int width, int height,bool applyHistogram);
    ~OpenCvFaceDetector();
private:
    vector<cv::Rect> mFaceRect;
    CascadeClassifier cascadeClassifier;
    DetectAngleOrientation detectAngleOrientation = DetectAngleOrientation::middle;
    Mat rotate(Mat& src, double angle);
    Mat histogram(Mat src, const float clip);

    int leftToRightAngle[3]={-24,0,24};
    int rightToLeftAngle[3]={24,0-24};
    int startWithMiddleAngle[3]={0,24,-24};


    int mAbsoluteFaceSize = 0,count = 0;
    float mRelativeFaceSize   = 0.20f;
    Mat imgMat;
    float clipLimit = 0.7f;
};

bool OpenCvFaceDetector::loadTrainData(std::string trainDataFilePath) {
    return cascadeClassifier.load(trainDataFilePath);
}

Mat OpenCvFaceDetector::rotate(Mat& src, double angle)
{

    auto start = std::chrono::high_resolution_clock::now();
    Mat dst;
    Point2f pt(src.cols >> 1, src.rows>>1);
    Mat r = getRotationMatrix2D(pt, angle, 1.0);
    warpAffine(src, dst, r, Size(src.cols, src.rows));

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    LOGI("..............rotate mat............... %lld",duration.count());
    return dst;
}

template <typename T>
std::string to_string(T value)
{
    std::ostringstream os ;
    os << value ;
    return os.str() ;
}



FaceDetectionResult OpenCvFaceDetector::detectFaces(unsigned char *imageBuf,int width, int height,bool applyHistogram) {
    Mat matIamge( height, width,CV_8UC1,imageBuf,Mat::AUTO_STEP);
    FaceDetectionResult* faceDetectionResult = new FaceDetectionResult();
    int height1 = matIamge.rows > matIamge.cols ? matIamge.rows : matIamge.cols;
    if (round(height1 * mRelativeFaceSize) > 0) {
            mAbsoluteFaceSize = round(height1 * mRelativeFaceSize);
        }
    int *anglearray ;

    switch (detectAngleOrientation){
        case DetectAngleOrientation ::right:{
            anglearray = rightToLeftAngle;
            break;
        }
        case DetectAngleOrientation ::middle:{
            anglearray = startWithMiddleAngle;
            break;
        }
        case DetectAngleOrientation ::left:{
            anglearray = leftToRightAngle;
            break;
        }
    }

    Size minSize = Size(mAbsoluteFaceSize, mAbsoluteFaceSize);
    Size maxSize = Size();


    for(int i=0;i<3;i++) {

        if (anglearray[i] != 0) {
            imgMat = rotate(matIamge, anglearray[i]);
        }
        else{
            imgMat = matIamge;
        }
        if(applyHistogram)
        imgMat = histogram(imgMat,clipLimit);
        //    auto start = std::chrono::high_resolution_clock::now();
        cascadeClassifier.detectMultiScale(imgMat, mFaceRect, 1.1, 3,
                                           0|CV_HAAR_SCALE_IMAGE,
                                           minSize,
                                           maxSize);

//        auto end = std::chrono::high_resolution_clock::now();
//    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//    LOGI("..............detect face............... %lld",duration.count());

        if (mFaceRect.size()>0) {
            if (anglearray[i] > 0){
                detectAngleOrientation = DetectAngleOrientation ::right;
            }
            else if (anglearray[i] < 0){
                detectAngleOrientation = DetectAngleOrientation ::left;
            }
            else{
                detectAngleOrientation = DetectAngleOrientation ::middle;
            }
            faceDetectionResult->isSuccessFull = true;
            faceDetectionResult->imageRotateAngle = anglearray[i];
            faceDetectionResult->imageBuf = imgMat.data;
            for (cv::Rect rect : mFaceRect) {
                facedetection::Rect rect1{rect.x,rect.y,rect.x + rect.width,rect.y + rect.height};
                faceDetectionResult->facesRect.push_back(rect1);
            }

            break;
        }
        else{
//            std::string str;
//            str.append("/sdcard/saved_images/");
//            str.append( to_string(count++));
//            str.append(".jpg");
//            imwrite(str , imgMat );
        }
    }

    return *faceDetectionResult;
}

Mat OpenCvFaceDetector::histogram(Mat src, const float clip) {
#ifdef DEBUG
    auto start = std::chrono::high_resolution_clock::now();
#endif
    Ptr<CLAHE> clahe = createCLAHE();
    Mat midProcess(src.rows, src.cols, CV_8UC1);
    clahe.get()->setClipLimit(clip);
    clahe.get()->setTilesGridSize(cv::Size(8, 8));
    clahe.get()->apply(src, midProcess);
    Mat midProcess1(src.rows, src.cols, CV_8UC1);
    clahe.get()->apply(midProcess, midProcess1);
#ifdef DEBUG
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    LOGI("..............Hist time............... %lld",duration.count());
#endif
    return midProcess1;
}

OpenCvFaceDetector::~OpenCvFaceDetector() {

}


TrainAbleDetector::~TrainAbleDetector() {

}


#endif //FACEDETECTIONANDROID_OPEN_CV_FACEDETECTOR_H_H
