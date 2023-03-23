//
// Created by 建 on 2017/03/11.
//

#ifndef MOBILEUSERTEST_LOG_H_H
#define MOBILEUSERTEST_LOG_H_H
#include <android/log.h>

#define LOG_TAG	"SAMFFmpeg-JNI"


#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGV(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

#endif //MOBILEUSERTEST_LOG_H_H
