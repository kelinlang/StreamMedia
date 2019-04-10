//
// Created by dengjun on 2019/3/21.
//


#include <sys/types.h>
#include <media/NdkMediaCodec.h>
#include <library/CommonLib/queue/cloudvoice_blocking_queue.h>
#include <malloc.h>
#include <library/media/model/cloudvoice_audio_video_model.h>
#include <library/CommonLib/queue/cloudvoice_list.h>
#include <memory.h>
#include <library/CommonLib/log/cloudvoice_log.h>
#include <pthread.h>
#include <media/NdkMediaFormat.h>
#include "cloudvoide_android_video_codec.h"

typedef  struct CloudVoiceCodecOpaque_{
    CloudVoiceCodecParam codecParam;
    int workFlag;
    pthread_t workThreadId;
    pthread_t readThreadId;

    AMediaCodec *mediaCodec;

    CloudVoiceBlockingQueue blockingQueue;
    CloudVoiceList cacheList;

    MediaStatusCallback mediaStatusCallback;
    MediaDataCallback mediaDataCallback;
}CloudVoiceCodecOpaque_;

static void cloudVoiceAVPackactFreeCallback(void *object, void* avPackect){
    cloudVoiceDestroyAVPackect((CloudVoiceAVPacket)avPackect);
}

static void cloudVoiceAVPackactFreeNodeCallback(CloudVoiceList object, void* avPackect){
    cloudVoiceDestroyAVPackect((CloudVoiceAVPacket)avPackect);
}


static void setParam(CloudVoiceCodec codec,CloudVoiceCodecParam codecParam){
    if(codec && codec->codecOpaque && codecParam){
        codec->codecOpaque->codecParam = codecParam;
    }
}


static CloudVoiceCodecParam getParam(CloudVoiceCodec codec){
    CloudVoiceCodecParam codecParam = NULL;
    if(codec && codec->codecOpaque ){
        codecParam = codec->codecOpaque->codecParam;
    }
    return codecParam;
}

static void setMediaStatusCallback(CloudVoiceCodec codec,MediaStatusCallback mediaStatusCallback){
    if(codec && codec->codecOpaque){
        codec->codecOpaque->mediaStatusCallback = mediaStatusCallback;
    }
}

static void setMediaDataCallback(CloudVoiceCodec codec,MediaDataCallback mediaDataCallback){
    if(codec && codec->codecOpaque){
        codec->codecOpaque->mediaDataCallback = mediaDataCallback;
    }
}

static void addData(CloudVoiceCodec codec,CloudVoiceAVPacket srcPackect){
    if(codec && codec ->codecOpaque && codec->codecOpaque->workFlag == 1){
        cloudVoiceLogD("codec add data avpackect : %p",srcPackect);
        codec->codecOpaque->blockingQueue->add(codec->codecOpaque->blockingQueue,srcPackect);
        cloudVoiceLogD("codec add data 1");

    } else{
        cloudVoiceDestroyAVPackect(srcPackect);
    }
}

static void codecInputData(CloudVoiceCodec codec){
    void * data = codec->codecOpaque->blockingQueue->take(codec->codecOpaque->blockingQueue);
    cloudVoiceLogD("codecInputData avpackect : %p",data);
    if(!data){
        return;
    }
    CloudVoiceAVPacket srcPackect = (CloudVoiceAVPacket)data;
    ssize_t bufidx = -1;
    if (srcPackect && srcPackect->data){
        bufidx = AMediaCodec_dequeueInputBuffer(codec->codecOpaque->mediaCodec,2000);
        if(bufidx >= 0){
            size_t bufsize;
            uint8_t* buf = AMediaCodec_getInputBuffer(codec->codecOpaque->mediaCodec,bufidx,&bufsize);
            cloudVoiceLogD("input data bufsize : %d,  dataLen : %d",bufsize,srcPackect->dataLen);
            memcpy(buf,srcPackect->data+srcPackect->startPos,srcPackect->dataLen);
            AMediaCodec_queueInputBuffer(codec->codecOpaque->mediaCodec,bufidx,0,srcPackect->dataLen,0,0);
        }
    }
    cloudVoiceLogD("codecInputData  destroy avpackect");
    cloudVoiceDestroyAVPackect(srcPackect);
}

static void codecOutputData(CloudVoiceCodec codec){
    ssize_t bufidx = -1;
    size_t bufsize;
    AMediaCodecBufferInfo info;

    bufidx = AMediaCodec_dequeueOutputBuffer(codec->codecOpaque->mediaCodec,&info,2000);
        cloudVoiceLogD("decode_func ing------------bufidx ： %d ",bufidx);
    if(bufidx >= 0){
        cloudVoiceLogD("decode_func ing------------1 ");

        uint8_t* buf = AMediaCodec_getOutputBuffer(codec->codecOpaque->mediaCodec,bufidx,&bufsize);
        cloudVoiceLogD("decode_func ing------------2  bufsize : %d",bufsize);

        if (codec->codecOpaque->mediaDataCallback){
            CloudVoiceAVPacket  voiceAVPackect = cloudVoiceCreateAVPackect();
            uint8_t *data =  (uint8_t*)malloc(bufsize);
            memcpy(data,buf,bufsize);
            voiceAVPackect->data = data;
            voiceAVPackect->dataLen = bufsize;

            //外部需要释放voiceAVPackect
            codec->codecOpaque->mediaDataCallback(codec->externHandle,NULL,0,voiceAVPackect,NULL);
        }

        AMediaCodec_releaseOutputBuffer(codec->codecOpaque->mediaCodec, bufidx, false);
    }
}

static int mediaCodecStart(CloudVoiceCodec codec){
    int retVal = -1;
    CloudVoiceCodecParam codecParam = codec->codecOpaque->codecParam;
    cloudVoiceLogI("codecParam videoDataWidth : %d , videoDataHeight : %d",codecParam->videoDataWidth,codecParam->videoDataHeight);

    AMediaCodec* mediaCodec = AMediaCodec_createDecoderByType("video/avc");

    AMediaFormat* videoFormat = AMediaFormat_new();
    AMediaFormat_setString(videoFormat, "mime", "video/avc");
    AMediaFormat_setInt32(videoFormat, AMEDIAFORMAT_KEY_WIDTH, codecParam->videoDataWidth); // 视频宽度
    AMediaFormat_setInt32(videoFormat, AMEDIAFORMAT_KEY_HEIGHT, codecParam->videoDataHeight); // 视频高度

    media_status_t status = AMediaCodec_configure(mediaCodec, videoFormat, NULL, NULL, 0);
    if (status != 0){
        cloudVoiceLogE("AMediaCodec_configure error : %d",status);
        AMediaCodec_stop(mediaCodec);
    } else{
        cloudVoiceLogI("-------------------------config media codec success-----------------");

        if ((status = AMediaCodec_start(mediaCodec)) != AMEDIA_OK){
            cloudVoiceLogE("AMediaCodec_start error : %d",status);
            AMediaCodec_stop(mediaCodec);
        } else{
            codec->codecOpaque->mediaCodec = mediaCodec;
            retVal = 0;
            cloudVoiceLogI("-------------------------mediaCodecStart success-----------------");
        }
    }
    AMediaFormat_delete(videoFormat);
    return retVal;
}

static  void* readThreadFunc(void *arg){
    CloudVoiceCodec codec = (CloudVoiceCodec)arg;
    cloudVoiceLogD("readThreadFunc loop start ");
    while (codec->codecOpaque->workFlag){
        void * data = codec->codecOpaque->blockingQueue->take(codec->codecOpaque->blockingQueue);
        cloudVoiceLogD("codecInputData avpackect : %p",data);
        if(!data){
            continue;
        }
        CloudVoiceAVPacket srcPackect = (CloudVoiceAVPacket)data;
        ssize_t bufidx = -1;
        if (srcPackect && srcPackect->data){
            bufidx = AMediaCodec_dequeueInputBuffer(codec->codecOpaque->mediaCodec,2000);
            if(bufidx >= 0){
                size_t bufsize;
                uint8_t* buf = AMediaCodec_getInputBuffer(codec->codecOpaque->mediaCodec,bufidx,&bufsize);
//                cloudVoiceLogD("input data bufsize : %d,  dataLen : %d",bufsize,srcPackect->dataLen);
                memcpy(buf,srcPackect->data+srcPackect->startPos,srcPackect->dataLen);
                AMediaCodec_queueInputBuffer(codec->codecOpaque->mediaCodec,bufidx,0,srcPackect->dataLen,0,0);
            }
        }
        cloudVoiceLogD("codecInputData  destroy avpackect");
        cloudVoiceDestroyAVPackect(srcPackect);
    }
}

static void* workThreadFunc(void *arg){
    CloudVoiceCodec codec = (CloudVoiceCodec)arg;

    ssize_t bufidx = -1;
    size_t bufsize;
    AMediaCodecBufferInfo info;
    cloudVoiceLogD("workThreadFunc loop start ");
    while (codec->codecOpaque->workFlag){
        bufidx = AMediaCodec_dequeueOutputBuffer(codec->codecOpaque->mediaCodec,&info,2000);
//        cloudVoiceLogD("decode_func ing------------bufidx ： %d ",bufidx);
        if(bufidx >= 0){
//            cloudVoiceLogD("decode_func ing------------1 ");

            uint8_t* buf = AMediaCodec_getOutputBuffer(codec->codecOpaque->mediaCodec,bufidx,&bufsize);
            cloudVoiceLogD("decode_func ing------------2  bufsize : %d",bufsize);

            if (codec->codecOpaque->mediaDataCallback){
                CloudVoiceAVPacket  voiceAVPackect = cloudVoiceCreateAVPackect();
                uint8_t *data =  (uint8_t*)malloc(bufsize);
                memcpy(data,buf,bufsize);
                voiceAVPackect->data = data;
                voiceAVPackect->dataLen = bufsize;

                //外部需要释放voiceAVPackect
                codec->codecOpaque->mediaDataCallback(codec->externHandle,NULL,0,voiceAVPackect,NULL);
            }

            AMediaCodec_releaseOutputBuffer(codec->codecOpaque->mediaCodec, bufidx, false);
        }

    }
    cloudVoiceLogI("codec thread  finish");
}

static void start(CloudVoiceCodec codec){
    if (codec && codec->codecOpaque){
        codec->codecOpaque->workFlag = 1;

        if (mediaCodecStart(codec) == 0){
            int retval = pthread_create(&codec->codecOpaque->workThreadId,NULL,workThreadFunc,codec);
            cloudVoiceLogI("start decode thread retval : %d", retval);

            int retval1 = pthread_create(&codec->codecOpaque->readThreadId,NULL,readThreadFunc,codec);
            cloudVoiceLogI("start decode read thread retval : %d", retval1);
        }
    }
}

static void stop(CloudVoiceCodec codec){
    if (codec && codec->codecOpaque){
        if(codec->codecOpaque->workThreadId != -1){
            cloudVoiceLogI("pthread_join  start");
            pthread_join(codec->codecOpaque->workThreadId,NULL);
            cloudVoiceLogI("pthread_join  finish");
            codec->codecOpaque->workThreadId = -1;
        }

        if(codec->codecOpaque->readThreadId != -1){
            cloudVoiceLogI("pthread_join  start 2");
            pthread_join(codec->codecOpaque->readThreadId,NULL);
            cloudVoiceLogI("pthread_join  finish 2");
            codec->codecOpaque->readThreadId = -1;
        }

        if(codec->codecOpaque->mediaCodec){
            AMediaCodec_stop(codec->codecOpaque->mediaCodec);
            codec->codecOpaque->mediaCodec = NULL;
        }
    }
}

static void destroy(CloudVoiceCodec codec){
    if (codec && codec->codecOpaque){
        stop(codec);
        if (codec->codecOpaque->blockingQueue){
            codec->codecOpaque->blockingQueue->destroy(codec->codecOpaque->blockingQueue);
            codec->codecOpaque->blockingQueue = NULL;
        }
        if (codec->codecOpaque->cacheList){
            codec->codecOpaque->cacheList->destroy(codec->codecOpaque->cacheList);
            codec->codecOpaque->cacheList = NULL;
        }
        if (codec->codecOpaque->codecParam){
            cloudVoiceDestroyCodecParam(codec->codecOpaque->codecParam);
            codec->codecOpaque->codecParam = NULL;
        }
        codec->externHandle = NULL;
        free(codec->codecOpaque);
        free(codec);
    }

}

CloudVoiceCodec cloudVoiceCreateAndroidVideoCodec(CloudVoiceCodecParam codecParam){
    CloudVoiceCodec codec = (CloudVoiceCodec)malloc(sizeof(CloudVoiceCodec_));
    if(codec){
        codec->getParam = getParam;
        codec->setParam = setParam;
        codec->setMediaStatusCallback = setMediaStatusCallback;
        codec->setMediaDataCallback = setMediaDataCallback;
        codec->addData = addData;
        codec->start = start;
        codec->stop = stop;
        codec->destroy = destroy;
        codec->externHandle = NULL;

        CloudVoiceCodecOpaque codecOpaque = (CloudVoiceCodecOpaque)malloc(sizeof(CloudVoiceCodecOpaque_));
        if (codecOpaque){
            codec->codecOpaque = codecOpaque;

            codecOpaque->blockingQueue = cloudVoiceCreateBlockingQueue(cloudVoiceAVPackactFreeCallback);
            codecOpaque->cacheList = cloudVoiceCreateList(cloudVoiceAVPackactFreeNodeCallback);

            codecOpaque->codecParam = cloudVoiceCreateCodecParam();
            codecOpaque->mediaDataCallback = NULL;
            codecOpaque->mediaStatusCallback = NULL;
            codecOpaque->workFlag = 0;
            codecOpaque->workThreadId = -1;
            codecOpaque->mediaCodec = NULL;
        }
    }
    return codec;
}