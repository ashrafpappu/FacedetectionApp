//
// Created by Pappu on 11/7/17.
//

#include "pappu_com_facedetection_java_jni_Facedetection.h"

#include <chrono>
#include "open_cv_facedetector.h"
#include <thread>
#include "FaceDetectionLogger.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


FaceDetectionResult FaceDetectionResultPtr;

OpenCvFaceDetector *openCvFaceDetector;
typedef struct _thread_data_t {
    unsigned char *imageBufSource;
    unsigned char *imageBufDestination;
    unsigned long rowstart;
    unsigned long imageWidth;
    unsigned long imageHeight;
    unsigned long rowinterval;
    int index;
} thread_data_t;



bool createNativeShapeDetector(JNIEnv *env, jobject instance, jstring filepath_) {
    TrainAbleDetector * trainAbleDetector = dynamic_cast<TrainAbleDetector*>(openCvFaceDetector);
    if (trainAbleDetector == nullptr) {
        LOGI("Not a train able detector");
        return true;
    }
    else {
        const char *filepath = env->GetStringUTFChars(filepath_, 0);
        bool flag = trainAbleDetector->loadTrainData(filepath);
        LOGI("%s load Successfully : %d",filepath,flag);
        env->ReleaseStringUTFChars(filepath_, filepath);
        return flag;
    }
}


void *rotateImageData(void *arg) {

    thread_data_t *data = (thread_data_t *) arg;
    int height = data->rowstart + data->rowinterval;

    for (int i = data->rowstart; i < height; i++) {
        for (int j = 0; j < data->imageWidth; j++) {
            data->imageBufDestination[data->index++] = data->imageBufSource[
                    ((data->imageWidth - 1) - j) * data->imageHeight +
                    (data->imageHeight - i - 1)];
        }
    }

    pthread_exit(NULL);

}


JNIEXPORT void JNICALL Java_pappu_com_facedetection_java_1jni_Facedetection_initialize
        (JNIEnv *, jobject){
        openCvFaceDetector =  new OpenCvFaceDetector();
}

JNIEXPORT jboolean JNICALL Java_pappu_com_facedetection_java_1jni_Facedetection_deserialize
        (JNIEnv *env, jobject object, jstring openCvXml){
    return createNativeShapeDetector(env,object,openCvXml);

}
JNIEXPORT jlongArray JNICALL Java_pappu_com_facedetection_java_1jni_Facedetection_getFaceRectangle
        (JNIEnv *env, jobject obj){
    jlongArray rect = env->NewLongArray(4);
    if(FaceDetectionResultPtr.isSuccessFull){
        jlong *cRect = new jlong[4];
        cRect[0] = FaceDetectionResultPtr.facesRect[0].left;
        cRect[1] = FaceDetectionResultPtr.facesRect[0].top;
        cRect[2] = FaceDetectionResultPtr.facesRect[0].right;
        cRect[3] = FaceDetectionResultPtr.facesRect[0].bottom;
        env->SetLongArrayRegion(rect, 0, 4, cRect);
        delete[] cRect;
    }

    return rect;

}
JNIEXPORT jboolean JNICALL Java_pappu_com_facedetection_java_1jni_Facedetection_faceDetect
        (JNIEnv *env, jobject object, jbyteArray image, jlong imageWidth, jlong imageHeight, jint orientation,jint coreNumber,jboolean applyHistogram){

    long height,width;
    int len = imageHeight * imageWidth;
    unsigned char *imageBuf = new unsigned char[len];

    if (orientation == 1) {
        height = imageWidth;
        width = imageHeight;

        unsigned char *imageBufSource = new unsigned char[len];
        env->GetByteArrayRegion(image, 0, len, reinterpret_cast<jbyte *>(imageBufSource));
#ifdef DEBUG
        auto start = std::chrono::high_resolution_clock::now();
#endif

        unsigned long rowstart = 0,rowinterval = height/coreNumber,index=0,indexinterval = (width*height)/coreNumber;

        pthread_t thr[coreNumber];
        int i;
        thread_data_t thr_data[coreNumber] ;

        for (i = 0; i < coreNumber; i++) {
            thr_data[i].imageBufDestination = imageBuf;
            thr_data[i].imageBufSource = imageBufSource;
            thr_data[i].imageWidth = width;
            thr_data[i].imageHeight = height;
            thr_data[i].rowinterval = rowinterval;
            if(i == 0){
                thr_data[i].rowstart = rowstart;
                thr_data[i].index = index;
            }
            else{

                thr_data[i].rowstart = rowstart+rowinterval;
                rowstart = thr_data[i].rowstart;
                thr_data[i].index = index+indexinterval;
                index =  thr_data[i].index;
            }
            pthread_create(&thr[i],NULL,rotateImageData,(void *) &thr_data[i]);
        }
        for (i = 0; i < coreNumber; ++i) {
            pthread_join(thr[i], NULL);
        }

        delete[] imageBufSource;
    }
    else {

        height = imageHeight;
        width = imageWidth;
        env->GetByteArrayRegion(image, 0, len, reinterpret_cast<jbyte *>(imageBuf));
    }
    auto start = std::chrono::high_resolution_clock::now();
    FaceDetectionResultPtr = openCvFaceDetector->detectFaces(imageBuf,width,height,applyHistogram);
    if (!FaceDetectionResultPtr.isSuccessFull)
    {
        delete[] imageBuf;
        return false;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    // LOGI("..............detect............... %lld",duration.count());


    return true;
}