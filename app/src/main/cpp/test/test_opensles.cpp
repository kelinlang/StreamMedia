//
// Created by dengjun on 2018/12/2.
//


#include "com_medialib_audio_base_OpenslEs.h"

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <stdlib.h>
#include <stdio.h>


// engine interfaces
static SLObjectItf engineObject = NULL;
static SLEngineItf engineEngine;

// output mix interfaces
static SLObjectItf outputMixObject = NULL;

// buffer queue player interfaces
static SLObjectItf bqPlayerObject = NULL;
static SLPlayItf bqPlayerPlay;
static SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
static SLEffectSendItf bqPlayerEffectSend;
static SLVolumeItf bqPlayerVolume;


static jmethodID playMethodId;
static jobject g_cls;
static jclass cls;
static jint audio_size = 3200;

static JNIEnv *playThreadEnv;

static void save_play_Thread_JniEnv(){
    if (!playThreadEnv){
        playThreadEnv = cl_android_jni_attach_current_thread_env();
    }
}

static void delete_play_Thread_JniEnv(){
    cl_android_jni_detach_current_thread_env();
    playThreadEnv = NULL;
}

static  jbyte *  getPlayData(){
    jbyte  data = (jbyte )playThreadEnv->CallStaticShortMethod(cls,playMethodId);
    return &data;
}

static void feedData(){
    if (bqPlayerBufferQueue != NULL){
        void *buffer = malloc(1600);
        (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, buffer, 1600);
    }
}

static void playerCallback(SLAndroidSimpleBufferQueueItf bq, void *context){
    LOGI("playerCallback");
    if(playThreadEnv == NULL){
        save_play_Thread_JniEnv();
    }

    if (playMethodId == NULL){
        if (cls == NULL){
            cls = playThreadEnv->GetObjectClass(g_cls);
        }
        if (cls != NULL ){
            LOGI("GetStaticMethodID");
            playMethodId = playThreadEnv->GetStaticMethodID(cls,"process_opensles_getPlayDataBytes","(I[B)I");

        } else{
            LOGI("cls == NULL");
        }
    }
    if (playMethodId != NULL){
        LOGI("--CallStaticObjectMethod--------------");

        void *audio_buf = malloc(audio_size);
        jbyteArray jbyteArray1 = playThreadEnv->NewByteArray(audio_size);
        playThreadEnv->SetByteArrayRegion(jbyteArray1,0,audio_size, (jbyte *) audio_buf);
        jint len = (jint)playThreadEnv->CallStaticObjectMethod(cls,playMethodId,audio_size,jbyteArray1);

        LOGI("len : %d",len);
        if (len == audio_size){
            (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, audio_buf, audio_size);
        }

        playThreadEnv->DeleteLocalRef(jbyteArray1);
    } else{
        LOGI("playMethodId == NULL");
    }

    feedData();
}

JNIEXPORT jint JNICALL Java_com_medialib_audio_base_OpenslEs_process_1opensles_1playstart
        (JNIEnv *env, jobject object) {
//    playThreadEnv = env;

    g_cls = env->NewGlobalRef(object);
//    playMethodId = env->GetStaticMethodID(cls, "process_opensles_getPlayDataBytes", "()[B");



    SLresult result;

    // create engine
    result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    LOGI("1 slCreateEngine  result :  %d ", result);
    // realize the engine
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    LOGI("2 engineObject Realize  result :  %d ", result);

    // get the engine interface, which is needed in order to create other objects
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    LOGI("3 GetInterface  result :  %d ", result);

    // create output mix,
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 0, 0, 0);
    LOGI("4 CreateOutputMix  result :  %d ", result);

    // realize the output mix
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    LOGI("5 outputMixObject  Realize result :  %d ", result);



    // configure audio source
    SLDataLocator_AndroidSimpleBufferQueue loc_bufq = { SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 3 };
    SLDataFormat_PCM format_pcm = { SL_DATAFORMAT_PCM, 1, SL_SAMPLINGRATE_16,
                                    SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
                                    SL_SPEAKER_FRONT_LEFT, SL_BYTEORDER_LITTLEENDIAN };
    SLDataSource audioSrc = { &loc_bufq, &format_pcm };

    // configure audio sink
    SLDataLocator_OutputMix loc_outmix = { SL_DATALOCATOR_OUTPUTMIX, outputMixObject };
    SLDataSink audioSnk = { &loc_outmix, NULL };

    // create audio player
    const SLInterfaceID ids[3] = { SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME };
    const SLboolean req[3] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };
    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &bqPlayerObject, &audioSrc, &audioSnk, 3, ids, req);
    LOGI("6 CreateAudioPlayer  result :  %d ", result);


    // realize the player
    result = (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE);
    LOGI("7 CreateAudioPlayer  Realize result :  %d ", result);


    // get the play interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY, &bqPlayerPlay);
    LOGI("8 bqPlayerObject  Realize result :  %d ", result);


    // get the buffer queue interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_BUFFERQUEUE, &bqPlayerBufferQueue);
    LOGI("9 bqPlayerObject  Realize result :  %d ", result);


    // register callback on the buffer queue
    result = (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue, playerCallback, NULL);
    LOGI("10 RegisterCallback  Realize result :  %d ", result);


    // get the effect send interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_EFFECTSEND, &bqPlayerEffectSend);
    LOGI("11 GetInterface  Realize result :  %d ", result);


    // get the volume interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_VOLUME, &bqPlayerVolume);
    LOGI("12 GetInterface  Realize result :  %d ", result);


    // set the player's state to playing
    result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
    LOGI("13 SetPlayState  Realize result :  %d ", result);
    //pthread_t id;


    feedData();
    LOGI("createAudioPlayer finish");

    return 0;
}





JNIEXPORT jint JNICALL Java_com_medialib_audio_base_OpenslEs_process_1opensles_1playstop
        (JNIEnv *env, jobject object) {
//destory player object
    if (bqPlayerObject != NULL) {
        (*bqPlayerObject)->Destroy(bqPlayerObject);
        bqPlayerPlay = NULL;
        bqPlayerBufferQueue = NULL;
        bqPlayerEffectSend = NULL;
        bqPlayerVolume = NULL;
    }

    // destroy output mix object, and invalidate all associated interfaces
    if (outputMixObject != NULL) {
        (*outputMixObject)->Destroy(outputMixObject);
        outputMixObject = NULL;
    }

    // destroy engine object, and invalidate all associated interfaces
    if (engineObject != NULL) {
        (*engineObject)->Destroy(engineObject);
        engineObject = NULL;
        engineEngine = NULL;
    }

    delete_play_Thread_JniEnv();

    if (g_cls != NULL){
        env->DeleteGlobalRef(g_cls);
    }
    return 0;
}