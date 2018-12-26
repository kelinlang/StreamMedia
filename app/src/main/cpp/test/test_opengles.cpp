//
// Created by dengjun on 2018/12/25.
//

#include "com_medialib_video_OpenGlEs.h"
#include "../CommonLib/thread/cl_sdl_thread.h"
#include "../StreamMedia/buffer/sm_video_data_buffer.h"
#include "../StreamMedia/display/sm_egl.h"
#include "../CommonLib/log/cl_sdl_log.h"
#include "../StreamMedia/model/sm_video_fourcc.h"


ClThread displayThread;
int displayThreadRunFlag = 0;
SmVideoDataQueue videoDataQueue;

SmVideoParam videoParam;


static int videoDisplayThread(void *arg){
    SmEGL egl = smCreateEgl();
    if (egl){
        egl->EglSetVideoParam(egl,videoParam);
        if (egl->EglInit(egl) == 0){
            LOGI("display loop start");
            while (displayThreadRunFlag){
                SmVideoDataNode videoDataNode = smVideoDataQueueDequeueData(videoDataQueue);
                if(videoDataNode){
                    egl->EglDisplay(egl,videoDataNode->videoData);
                    smCacheVideoDataNodeToCache(videoDataQueue,videoDataNode);
                }
            }
            LOGI("display loop finish");
        }
        egl->EglRelease(egl);
    }
}



JNIEXPORT jint JNICALL Java_com_medialib_video_OpenGlEs_start
        (JNIEnv * env, jobject object){
    LOGI("display  start");

    videoParam = smCreateVideoParam();
    videoParam->viewWidth = 640;
    videoParam->viewHeight= 480;
    videoParam->displayFormat = SM_VIDEO_FCC_I420;

    videoDataQueue = smCreateVideoDataQueue();


    displayThreadRunFlag = 1;
    displayThread = clCreateThread(videoDisplayThread,NULL,"display thread");

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
        smDestroyVideoParam(videoParam);
        videoParam = NULL;
    }

    if (videoDataQueue){
        smDestroyVideoDataQueue(videoDataQueue);
    }

}

/*
 * Class:     com_medialib_video_OpenGlEs
 * Method:    sendYuvData
 * Signature: (I[BI)V
 */
JNIEXPORT void JNICALL Java_com_medialib_video_OpenGlEs_sendYuvData
        (JNIEnv * env, jobject object, jint yunType, jbyteArray yuvData, jint yuvDataLen){

    if(videoDataQueue) {
        SmVideoDataNode videoDataNode = smCreateVideoDataNodeFromCache(videoDataQueue);
        if(videoDataNode){
            SmVideoData  videoData = videoDataNode->videoData;
            if(videoData->pixels){
                //如果没有存储空间，则分配
            }
            //复制数据


            //数据放到队列
        }
    }

}