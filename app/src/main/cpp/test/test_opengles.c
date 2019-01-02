//
// Created by dengjun on 2018/12/25.
//

#include "../StreamMedia/model/sm_video_fourcc.h"
#include "../StreamMedia/model/sm_video_data.h"
#include "../CommonLib/thread/cl_sdl_thread.h"
#include "../StreamMedia/buffer/sm_video_data_buffer.h"
#include "../StreamMedia/display/sm_egl.h"

#include "../CommonLib/log/cl_sdl_log.h"
#include "com_medialib_video_OpenGlEs.h"

#include <android/native_window_jni.h>
#include <string.h>





ClThread displayThread;
int displayThreadRunFlag = 0;
SmVideoDataQueue videoDataQueue;

SmVideoParam videoParam;
ANativeWindow* mANativeWindow;

static int videoDisplayThread(void *arg){
    LOGI("videoDisplayThread 0");
    SmEGL egl = smCreateEgl();
    if (egl){
        LOGI("videoDisplayThread 1");

        egl->window = mANativeWindow;
        egl->EglSetVideoParam(egl,videoParam);
        if (egl->EglInit(egl) == 0){
            LOGI("display loop start");
            while (displayThreadRunFlag){
                LOGI("display loop ------------- 1");
                SmVideoDataNode videoDataNode = smVideoDataQueueDequeueData(videoDataQueue);
                if(videoDataNode){
                    LOGI("display loop ------------ 2");
                    egl->EglDisplay(egl,videoDataNode->videoData);
                    smCacheVideoDataNodeToCache(videoDataQueue,videoDataNode);
                }
                LOGI("display loop ------------ 4");
            }
            LOGI("display loop finish");
        }
        LOGI("videoDisplayThread 2");
        egl->EglRelease(egl);
    }
    LOGI("videoDisplayThread finish");
    return 0;
}


JNIEXPORT jint JNICALL Java_com_medialib_video_OpenGlEs_setSurface
        (JNIEnv * env, jobject object, jobject surface){
    mANativeWindow = ANativeWindow_fromSurface(env, surface);
    return 0;
}

JNIEXPORT jint JNICALL Java_com_medialib_video_OpenGlEs_start
        (JNIEnv * env, jobject object){
    LOGI("display  start");
    cl_init_log();

    videoParam = smCreateVideoParam();
    videoParam->viewWidth = 640;
    videoParam->viewHeight= 480;
    videoParam->displayFormat = SM_VIDEO_FCC_I420;

    videoDataQueue = smCreateVideoDataQueue();


    displayThreadRunFlag = 1;
    displayThread = clCreateThread(videoDisplayThread,NULL,"display thread");
    return 0;
}

/*
 * Class:     com_medialib_video_OpenGlEs
 * Method:    stop
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_medialib_video_OpenGlEs_stop
        (JNIEnv * env, jobject object){
    LOGI("display  stop");

    if (displayThread){
        int status;
        displayThreadRunFlag = 0;
        clWaitThread(displayThread,&status);
        LOGI("display thread  finish status : %d", status);
        displayThread = NULL;
    }

    if (videoParam){
        LOGI("smDestroyVideoParam  start");

        smDestroyVideoParam(videoParam);
        videoParam = NULL;
        LOGI("smDestroyVideoParam  finish");

    }

    if (videoDataQueue){
        LOGI("smDestroyVideoDataQueue  start");
        smDestroyVideoDataQueue(videoDataQueue);
        videoDataQueue = NULL;
        LOGI("smDestroyVideoDataQueue  finish");
    }

    return 0;
}

/*
 * Class:     com_medialib_video_OpenGlEs
 * Method:    sendYuvData
 * Signature: (I[BI)V
 */
JNIEXPORT void JNICALL Java_com_medialib_video_OpenGlEs_sendYuvData
        (JNIEnv * env, jobject object, jint yunType, jbyteArray yuvData, jint yuvDataLen){
    jbyte* yuvJniData = (*env)->GetByteArrayElements(env,yuvData, NULL);

    if(videoDataQueue) {
        SmVideoDataNode videoDataNode = smCreateVideoDataNodeFromCache(videoDataQueue);
        if(videoDataNode){
//            LOGI("sendYuvData  ------------ 1");

            SmVideoData  videoData = videoDataNode->videoData;
            videoData->pitches[0] = videoParam->viewWidth*videoParam->viewHeight;
            videoData->pitches[1] = videoParam->viewWidth*videoParam->viewHeight/4;
            videoData->pitches[2] = videoParam->viewWidth*videoParam->viewHeight/4;

//            if(videoData->pixelsY){
                //如果没有存储空间，则分配
                videoData->pixelsY = (unsigned char*)malloc(videoData->pitches[0]);
//            }
//            LOGI("sendYuvData  ------------ 2");

//            if(videoData->pixelsU){
                //如果没有存储空间，则分配
                videoData->pixelsU = (unsigned char*)malloc(videoData->pitches[1]);

//            }
//            LOGI("sendYuvData  ------------ 3");

//            if(videoData->pixelsV){
                //如果没有存储空间，则分配
                videoData->pixelsV = (unsigned char*)malloc(videoData->pitches[2]);

//            }
//            LOGI("sendYuvData  ------------ 4");

            //复制数据
            memcpy(videoData->pixelsY, yuvJniData, videoData->pitches[0]);
            memcpy(videoData->pixelsY, yuvJniData+videoData->pitches[0], videoData->pitches[1]);
            memcpy(videoData->pixelsY, yuvJniData+videoData->pitches[0]+videoData->pitches[2], videoData->pitches[2]);

//            LOGI("sendYuvData  ------------ 5");
            //数据放到队列
            smVideoDataQueueEnqueueData(videoDataQueue,videoDataNode);
//            LOGI("sendYuvData  ------------ 6");
        }
    }

    (*env)->ReleaseByteArrayElements(env,yuvData,yuvJniData,0);
}