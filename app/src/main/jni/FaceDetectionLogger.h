
#include <android/log.h>

#ifndef FACEDETECTIONANDROID_FACEDETECTIONLOGGER_H
#define FACEDETECTIONANDROID_FACEDETECTIONLOGGER_H

#define DEBUG;

#ifdef DEBUG

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,"FDLogger",__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,"FDLogger",__VA_ARGS__)

#else

#define LOGD(...)
#define LOGI(...)

#endif

#endif //FACEDETECTIONANDROID_FACEDETECTIONLOGGER_H
