//
// Created by dengjun on 2018/10/18.
//

#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <cstdlib>
#include "com_stream_media_demo_nativelib_StreamMediaNative.h"
#include "log.h"
#include "pthread.h"
#include "media/NdkMediaCodec.h"
#include "stdio.h"

typedef struct {
    jboolean encodeThreadRunFlag;
    pthread_t encodeThreadId;
    AMediaCodec *encode;
} workerdata;

workerdata data = {JNI_FALSE, -1,NULL};
workerdata* pdata = &data;



void create_encode(){
    pdata->encode = AMediaCodec_createEncoderByType("video/avc");
    AMediaFormat* videoFormat = AMediaFormat_new();
    AMediaFormat_setString(videoFormat, "mime", "video/avc");
    AMediaFormat_setInt32(videoFormat, AMEDIAFORMAT_KEY_WIDTH, 640); // 视频宽度
    AMediaFormat_setInt32(videoFormat, AMEDIAFORMAT_KEY_HEIGHT, 480); // 视频高度


    int bitrate = 500000;
    int framerate = 5;
    AMediaFormat_setInt32(videoFormat, AMEDIAFORMAT_KEY_BIT_RATE, bitrate);
    AMediaFormat_setInt32(videoFormat, AMEDIAFORMAT_KEY_FRAME_RATE, framerate);
    AMediaFormat_setInt32(videoFormat, AMEDIAFORMAT_KEY_I_FRAME_INTERVAL, framerate);
//    AMediaFormat_setInt32(videoFormat, AMEDIAFORMAT_KEY_COLOR_FORMAT, 19); // COLOR_FormatYUV420Planar = 19
    AMediaFormat_setInt32(videoFormat, AMEDIAFORMAT_KEY_COLOR_FORMAT, 21);
    media_status_t status = AMediaCodec_configure(pdata->encode, videoFormat, NULL, NULL, AMEDIACODEC_CONFIGURE_FLAG_ENCODE);
    if (status != 0) {
        LOGE("AMediaCodec_configure() failed with error %i for format %u", (int)status, 21);
    } else {
        if ((status = AMediaCodec_start(pdata->encode)) != AMEDIA_OK) {
            LOGE("AMediaCodec_start: Could not start encoder.");
        } else {
            LOGD("AMediaCodec_start: encoder successfully started");
        }
        LOGD("status : %d",status);
    }
    AMediaFormat_delete(videoFormat);
//    AMediaCodec_configure(pdata->encode,videoFormat,NULL,NULL,AMEDIACODEC_CONFIGURE_FLAG_ENCODE);




//    media_status_t media_status =  AMediaCodec_start(pdata->encode);
//    LOGD("media_status_t : %d",media_status);
}

void close_encode(){
    LOGD("close_encode");
    if(pdata->encode != NULL){
        AMediaCodec_stop(pdata->encode);
        AMediaCodec_delete(pdata->encode);
    }
    pdata->encode = NULL;
}

void queue_data(uint8_t *data){
    AMediaCodec * encode = pdata ->encode;
    ssize_t bufidx = -1;
    if(encode != NULL){
//        LOGD("input data 1");
        bufidx = AMediaCodec_dequeueInputBuffer(encode,2000);
        if(bufidx >= 0){
//            LOGD("input data 1");
            size_t bufsize;
            uint8_t* buf = AMediaCodec_getInputBuffer(encode,bufidx,&bufsize);
            memcpy(data,buf,bufsize);
            AMediaCodec_queueInputBuffer(encode,bufidx,0,bufsize,0,0);
        }
    }
}

int fileFd = -1;
void openSave(){
    char  *path = "/sdcard/h264Tmp.h264";
    if (access(path,F_OK) != -1){
        remove(path);
    }
    fileFd = open(path,O_RDWR | O_CREAT);
}

void closeSave(){
    if(fileFd != -1){
        close(fileFd);
    }
}

void write(uint8_t* buf, size_t size){
    write(fileFd,buf,size);
}

void* encode_func(void *arg){
    LOGD("encode_func loop start");
    ssize_t bufidx = -1;
    AMediaCodec * encode = pdata ->encode;
    AMediaCodecBufferInfo info;
    size_t bufsize;

    openSave();

    uint8_t * config_data;
    size_t config_data_size;
    while (pdata->encodeThreadRunFlag){
        bufidx = AMediaCodec_dequeueOutputBuffer(encode,&info,2000);
        if(bufidx >= 0){
            uint8_t* buf = AMediaCodec_getOutputBuffer(encode,bufidx,&bufsize);
            switch (info.flags){
                case AMEDIACODEC_BUFFER_FLAG_CODEC_CONFIG:
                    LOGD("info.flags : %d",info.flags);
                    config_data_size = bufsize;
                    config_data = (uint8_t*)malloc(bufsize);
                    memcpy(buf,config_data,bufsize);
                    break;
                case 1:
//                    LOGD("info.flags : %d",info.flags);
                    write(config_data,config_data_size);
                    write(buf,bufsize);
                    break;
                default:
                    write(buf,bufsize);
                    break;
            }
            AMediaCodec_releaseOutputBuffer(encode, bufidx, false);
        }
    }
    if(config_data != NULL){
        free(config_data);
    }
    AMediaCodec_flush(pdata->encode);
    closeSave();
    LOGD("encode_func loop stop");
    pthread_exit(NULL);
}



void start_encode_thread(){
    pdata->encodeThreadRunFlag = JNI_TRUE;
    int retval = pthread_create(&data.encodeThreadId,NULL,encode_func,pdata);
    LOGD("retval : %d", retval);

}

void stop_encode_thread() {
    pdata->encodeThreadRunFlag = JNI_FALSE;
    if (pdata->encodeThreadId != -1){
        LOGD("pthread_join  start");
        pthread_join(data.encodeThreadId,NULL);
        LOGD("pthread_join  finish");
    }
    pdata->encodeThreadId = -1;
}






/*
 * Class:     com_stream_media_demo_nativelib_StreamMediaNative
 * Method:    setSurface
 * Signature: (Landroid/view/Surface;)V
 */
JNIEXPORT void JNICALL Java_com_stream_media_demo_nativelib_StreamMediaNative_setSurface
        (JNIEnv *env, jclass jclass, jobject jobject){

}

/*
 * Class:     com_stream_media_demo_nativelib_StreamMediaNative
 * Method:    init
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_stream_media_demo_nativelib_StreamMediaNative_init
        (JNIEnv *env, jclass jclass){
    create_encode();
    start_encode_thread();
}

/*
 * Class:     com_stream_media_demo_nativelib_StreamMediaNative
 * Method:    release
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_stream_media_demo_nativelib_StreamMediaNative_release
        (JNIEnv *env, jclass jclass){
    stop_encode_thread();
    close_encode();
}

/*
 * Class:     com_stream_media_demo_nativelib_StreamMediaNative
 * Method:    pushVideoData
 * Signature: ([BLcom/stream/media/demo/camera/VideoDataInfo;)V
 */
JNIEXPORT void JNICALL Java_com_stream_media_demo_nativelib_StreamMediaNative_pushVideoData
        (JNIEnv *env, jclass jclass, jbyteArray data, jobject jobject){
//    LOGD("test : %s","ee");
    unsigned char * buffer = (unsigned char *)env->GetByteArrayElements(data, 0);
    queue_data(buffer);
    env->ReleaseByteArrayElements(data, (jbyte*)buffer, 0);
}