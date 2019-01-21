//
// Created by dengjun on 2019/1/18.
//

#include <malloc.h>
#include <pthread.h>
#include <string.h>
#include "sm_mediacodec_test.h"
#include "../../CommonLib/log/cl_sdl_log.h"
#include "../buffer/sm_video_data_buffer.h"

typedef struct SmMediaCodec_{
    jboolean encodeThreadRunFlag;
    pthread_t encodeThreadId;
    AMediaCodec *decode;

    DataCallback  dataCallback;

    SmVideoDataQueue videoDataQueue;

}SmMediaCodec_;



SmMediaCodec smCreateMediaCodec(){
    SmMediaCodec mediaCodec = (SmMediaCodec)malloc(sizeof(SmMediaCodec_));
    if (mediaCodec){

    }
    return mediaCodec;
}




void smMediaCodecSetDataCallback(SmMediaCodec mediaCodec, DataCallback dataCallback){
    if (mediaCodec){
        mediaCodec->dataCallback = dataCallback;
    }
}

static  void* decode_func(void *arg){
    LOGI("decode_func start");
    SmMediaCodec  mediaCodec = (SmMediaCodec)arg;

    ssize_t bufidx = -1;
    size_t bufsize;
    AMediaCodecBufferInfo info;

    while (mediaCodec->encodeThreadRunFlag == 1){
        ssize_t bufidx = -1;

        SmVideoDataNode videoDataNode = smVideoDataQueueDequeueData(mediaCodec->videoDataQueue);

        bufidx = AMediaCodec_dequeueInputBuffer(mediaCodec->decode,2000);
//        LOGI("input data bufidx : %d",bufidx);
        if(bufidx >= 0){
            size_t bufsize;
            uint8_t* buf = AMediaCodec_getInputBuffer(mediaCodec->decode,bufidx,&bufsize);
            memcpy(buf,videoDataNode->videoData->pixelsData,videoDataNode->videoData->pixelsDataLen);
            AMediaCodec_queueInputBuffer(mediaCodec->decode,bufidx,0,videoDataNode->videoData->pixelsDataLen,0,0);
        }

        smCacheVideoDataNodeToCache(mediaCodec->videoDataQueue,videoDataNode);


        bufidx = AMediaCodec_dequeueOutputBuffer(mediaCodec->decode,&info,2000);
//        LOGI("decode_func ing------------bufidx ： %d ",bufidx);
        if(bufidx >= 0){
//            LOGI("decode_func ing------------1 ");

            uint8_t* buf = AMediaCodec_getOutputBuffer(mediaCodec->decode,bufidx,&bufsize);
//            LOGI("decode_func ing------------2  bufsize : %d",bufsize);

            if (mediaCodec ->dataCallback){
                mediaCodec->dataCallback(0,bufsize,buf,NULL);
            }

            AMediaCodec_releaseOutputBuffer(mediaCodec->decode, bufidx, false);
        }
    }

    LOGI("decode_func finish");

}



void smMediaCodecstart(SmMediaCodec mediaCodec){
    if(mediaCodec){
        mediaCodec->encodeThreadRunFlag = 1;

        mediaCodec->decode = AMediaCodec_createDecoderByType("video/avc");
        AMediaFormat* videoFormat = AMediaFormat_new();
        AMediaFormat_setString(videoFormat, "mime", "video/avc");
        AMediaFormat_setInt32(videoFormat, AMEDIAFORMAT_KEY_WIDTH, 640); // 视频宽度
        AMediaFormat_setInt32(videoFormat, AMEDIAFORMAT_KEY_HEIGHT, 480); // 视频高度

        int bitrate = 500000;
        int framerate = 5;
//        AMediaFormat_setInt32(videoFormat, AMEDIAFORMAT_KEY_BIT_RATE, bitrate);
//        AMediaFormat_setInt32(videoFormat, AMEDIAFORMAT_KEY_FRAME_RATE, framerate);
//        AMediaFormat_setInt32(videoFormat, AMEDIAFORMAT_KEY_I_FRAME_INTERVAL, framerate);
//        AMediaFormat_setInt32(videoFormat, AMEDIAFORMAT_KEY_COLOR_FORMAT, 19);

        media_status_t status = AMediaCodec_configure(mediaCodec->decode, videoFormat, NULL, NULL, 0);
        if (status != 0){
            LOGI("config media codec error");
        } else{
            LOGI("-------------------------config media codec success-----------------");
        }

        if ((status = AMediaCodec_start(mediaCodec->decode)) != AMEDIA_OK) {
            LOGE("AMediaCodec_start: Could not start decode.");
        } else{
            LOGD("AMediaCodec_start: decode successfully started");

            int retval = pthread_create(&mediaCodec->encodeThreadId,NULL,decode_func,mediaCodec);
//
            LOGD("start decode thread retval : %d", retval);
        }

        AMediaFormat_delete(videoFormat);

        mediaCodec->videoDataQueue = smCreateVideoDataQueue();
    }
}

void smMediaCodecstop(SmMediaCodec mediaCodec){
    LOGD("close_encode");
    if (mediaCodec){
        mediaCodec->encodeThreadRunFlag = 0;
        if(mediaCodec->encodeThreadId != -1){
            LOGD("pthread_join  start");
            pthread_join(mediaCodec->encodeThreadId,NULL);
            LOGD("pthread_join  finish");
            mediaCodec->encodeThreadId = -1;
        }
        if(mediaCodec->decode){
            AMediaCodec_stop(mediaCodec->decode);
        }
    }
}
void smMediaCodecRelease(SmMediaCodec mediaCodec){
    smMediaCodecstop(mediaCodec);
    if (mediaCodec ){
        if (mediaCodec->decode){
            AMediaCodec_delete(mediaCodec->decode);
        }

        if(mediaCodec->videoDataQueue){
            smDestroyVideoDataQueue(mediaCodec->videoDataQueue);
        }

        free(mediaCodec);
    }

}

void smMediaCodecAddData(SmMediaCodec mediaCodec, uint8_t *h264Data,int h264DataLen){
    if (mediaCodec && mediaCodec->decode && mediaCodec->encodeThreadRunFlag == 1){
//        LOGI("input data bufidx ---------------------------------------------");
        SmVideoDataNode videoDataNode = smCreateVideoDataNodeFromCache(mediaCodec->videoDataQueue);

        SmVideoData videoData = videoDataNode->videoData;

        if (videoData->pixelsData == NULL) {
            //如果没有存储空间，则分配
            videoData->pixelsDataLen = h264DataLen;
            videoData->pixelsData = (unsigned char *) malloc(h264DataLen);
        }

        memcpy(videoData->pixelsData, h264Data, h264DataLen);
        smVideoDataQueueEnqueueData(mediaCodec->videoDataQueue, videoDataNode);











        /*ssize_t bufidx = -1;
        bufidx = AMediaCodec_dequeueInputBuffer(mediaCodec->decode,2000);
        LOGI("input data bufidx : %d",bufidx);
        if(bufidx >= 0){
            size_t bufsize;
            uint8_t* buf = AMediaCodec_getInputBuffer(mediaCodec->decode,bufidx,&bufsize);
            memcpy(h264Data,buf,bufsize);
            AMediaCodec_queueInputBuffer(mediaCodec->decode,bufidx,0,bufsize,0,0);
        }*/
    }
}