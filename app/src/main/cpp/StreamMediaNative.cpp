//
// Created by dengjun on 2018/10/18.
//

#include <unistd.h>
#include <cstring>
#include "com_stream_media_demo_nativelib_StreamMediaNative.h"
#include "log.h"
#include "pthread.h"
#include "media/NdkMediaCodec.h"

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
    AMediaFormat_setInt32(videoFormat, AMEDIAFORMAT_KEY_WIDTH, 1280); // 视频宽度
    AMediaFormat_setInt32(videoFormat, AMEDIAFORMAT_KEY_HEIGHT, 720); // 视频高度

    AMediaFormat_setInt32(videoFormat, AMEDIAFORMAT_KEY_COLOR_FORMAT, 19); // COLOR_FormatYUV420Planar = 19

    AMediaCodec_configure(pdata->encode,videoFormat,NULL,NULL,AMEDIACODEC_CONFIGURE_FLAG_ENCODE);
    AMediaCodec_start(pdata->encode);
}

void close_encode(){
    AMediaCodec_stop(pdata->encode);
    pdata->encode = NULL;
}

void queue_data(uint8_t *data){
    AMediaCodec * encode = pdata ->encode;
    ssize_t bufidx = -1;
    if(encode != NULL){
        bufidx = AMediaCodec_dequeueInputBuffer(encode,2000);
        if(bufidx >= 0){
            size_t bufsize;
            uint8_t* buf = AMediaCodec_getInputBuffer(encode,bufidx,&bufsize);
            memcpy(data,buf,bufsize);
            AMediaCodec_queueInputBuffer(encode,bufidx,0,bufsize,0,0);
        }
    }
}


void* encode_func(void *arg){
    LOGD("encode_func loop start");
    ssize_t bufidx = -1;
    AMediaCodec * encode = pdata ->encode;
    AMediaCodecBufferInfo info;
    size_t bufsize;
    while (pdata->encodeThreadRunFlag){
        bufidx = AMediaCodec_dequeueOutputBuffer(encode,&info,2000);
        if(bufidx >= 0){
            uint8_t* buf = AMediaCodec_getOutputBuffer(encode,bufidx,&bufsize);
            LOGD("info.flags : %d",info.flags);
            switch (info.flags){
                case AMEDIACODEC_BUFFER_FLAG_CODEC_CONFIG:
                    break;
                case 1:
                    break;
                default:
                    break;
            }
        }
    }
    LOGD("encode_func loop stop");
}



void start_encode_thread(){
    pdata->encodeThreadRunFlag = JNI_TRUE;
    int retval = pthread_create(&data.encodeThreadId,NULL,encode_func,pdata);
    LOGD("retval : %d", retval);

}

void stop_encode_thread() {
    pdata->encodeThreadRunFlag = JNI_FALSE;
    if (pdata->encodeThreadId != -1){
        pthread_join(data.encodeThreadId,NULL);
    }
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