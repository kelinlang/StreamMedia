//
// Created by dengjun on 2018/11/15.
//

#ifndef STREAMMEDIADEMO_CL_SDL_LOG_H
#define STREAMMEDIADEMO_CL_SDL_LOG_H


#include <android/log.h>
#include <jni.h>

#define TAG "RealTimeVoice"
static jboolean debug = JNI_TRUE;

void setDebug(jboolean isDebug) {
    debug = isDebug;
}

#define LOGV(...) if (debug) __android_log_print(ANDROID_LOG_VERBOSE,TAG ,__VA_ARGS__)
#define LOGD(...) if (debug) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__)
#define LOGI(...) if (debug) __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__)
#define LOGW(...) if (debug) __android_log_print(ANDROID_LOG_WARN,TAG ,__VA_ARGS__)
#define LOGE(...) if (debug) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__)



#endif //STREAMMEDIADEMO_CL_SDL_LOG_H
