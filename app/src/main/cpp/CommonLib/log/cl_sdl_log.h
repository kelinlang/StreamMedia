//
// Created by dengjun on 2018/11/15.
//

#ifndef STREAMMEDIADEMO_CL_SDL_LOG_H
#define STREAMMEDIADEMO_CL_SDL_LOG_H


#include <android/log.h>
#include <jni.h>
#include "cl_sdl_abstract_log.h"
#include <stdlib.h>

#define TAG "RealTimeVoice"
static jboolean DEBUG_FLAG = JNI_FALSE;
static jboolean LOG_V_FLAG = JNI_TRUE;

void setDebug(jboolean isDebug);

#define LOGV(...) if (LOG_V_FLAG) __android_log_print(ANDROID_LOG_VERBOSE,TAG ,__VA_ARGS__)
#define LOGD(...) if (DEBUG_FLAG) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__)
#define LOGI(...) if (LOG_V_FLAG) __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__)
#define LOGW(...) if (LOG_V_FLAG) __android_log_print(ANDROID_LOG_WARN,TAG ,__VA_ARGS__)
#define LOGE(...) if (LOG_V_FLAG) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__)



void cl_init_log();


#endif //STREAMMEDIADEMO_CL_SDL_LOG_H
