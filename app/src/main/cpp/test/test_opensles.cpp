//
// Created by dengjun on 2018/12/2.
//


#include "com_medialib_audio_base_OpenslEs.h"

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>


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

/*static  jbyte *  getPlayData(){
    jbyte  data = (jbyte )playThreadEnv->CallStaticShortMethod(cls,playMethodId);
    return &data;
}*/

static void feedData(){
    if (bqPlayerBufferQueue != NULL){
        void *buffer = malloc(160);
        (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, buffer, 160);
    }
}

static void test(JNIEnv *env, jobject jobj){
    jclass jclass1 = env->GetObjectClass(jobj);
    jmethodID mid = env->GetMethodID(jclass1,"printMessage", "(I)V");
    env->CallVoidMethod(jobj,mid,10);
}

static void testStatic(JNIEnv *env, jobject jobj){
    if (env == NULL){
        LOGI("testStatic 1 env == NULL");
        return;
    }

    if (jobj == NULL){
        LOGI("testStatic 1 jobj == NULL");
        return;
    }

    jclass jclass1 = env->GetObjectClass(jobj);
    jmethodID mid = env->GetStaticMethodID(jclass1,"printLog", "(Ljava/lang/String;)I");
    jstring jstr = env->NewStringUTF("JNI invoke Java Static Method");
    jint len = (jint)env->CallStaticIntMethod(jclass1, mid, jstr);
    LOGI("len : %d",len);
}

static void testGetString(JNIEnv *env, jobject jobj){
    if (env == NULL){
        LOGI("testStatic 1 env == NULL");
        return;
    }

    if (jobj == NULL){
        LOGI("testStatic 1 jobj == NULL");
        return;
    }

    jclass jclass1 = env->GetObjectClass(jobj);
    LOGI("getString  : %s","start");

    jmethodID mid = env->GetStaticMethodID(jclass1,"getString", "()Ljava/lang/String;");
    LOGI("getString  : %s","start 1");

    jstring len = (jstring)env->CallStaticObjectMethod(jclass1, mid);
    LOGI("getString  : %s",len);
}

static void getPlayDataBytes(JNIEnv *env, jobject jobj){
    if (env == NULL){
        LOGI("testStatic 1 env == NULL");
        return;
    }

    if (jobj == NULL){
        LOGI("testStatic 1 jobj == NULL");
        return;
    }

    jclass jclass1 = env->GetObjectClass(jobj);
    jmethodID mid = env->GetStaticMethodID(jclass1,"getPlayDataBytes", "()[B");
    jbyteArray  jbyteArray1 = (jbyteArray)env->CallStaticObjectMethod(jclass1, mid);

    unsigned char * data = (unsigned char *)env->GetByteArrayElements(jbyteArray1, 0);
    if (bqPlayerBufferQueue != NULL){
        LOGI("add data");
        (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, data, 3200);
    }
    env->DeleteLocalRef(jbyteArray1);
    env->DeleteLocalRef(jclass1);
}

static void getDPlayDataBytes(JNIEnv *env, jobject jobj){
    if (env == NULL){
        LOGI("testStatic 1 env == NULL");
        return;
    }

    if (jobj == NULL){
        LOGI("testStatic 1 jobj == NULL");
        return;
    }
    jint audio_size = 1600;
    jshort *audio_buf = (jshort*)malloc(audio_size);
    jbyteArray jbyteArray1 = playThreadEnv->NewByteArray(audio_size);
    playThreadEnv->SetByteArrayRegion(jbyteArray1,0,audio_size, (jbyte *) audio_buf);

    jclass jclass1 = env->GetObjectClass(jobj);
    jmethodID mid = env->GetStaticMethodID(jclass1,"process_opensles_getPlayDataBytes", "(I[B)I");
    jint len = env->CallStaticIntMethod(jclass1, mid,audio_size,jbyteArray1);
    LOGI("len : %d",len);


    if (bqPlayerBufferQueue != NULL){
        LOGI("add data");
        (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, audio_buf, audio_size);
    }
    env->DeleteLocalRef(jbyteArray1);
    env->DeleteLocalRef(jclass1);
}

static void getShortsPlayDataBytes(JNIEnv *env, jobject jobj){
    if (env == NULL){
        LOGI("testStatic 1 env == NULL");
        return;
    }

    if (jobj == NULL){
        LOGI("testStatic 1 jobj == NULL");
        return;
    }

    jint audio_size = 1600;
    jshort *audio_buf = (jshort*) malloc(audio_size);
    jshortArray jshortArray1 = playThreadEnv->NewShortArray(audio_size);
    playThreadEnv->SetShortArrayRegion(jshortArray1,0,audio_size, audio_buf);

    jclass jclass1 = env->GetObjectClass(jobj);
    jmethodID mid = env->GetStaticMethodID(jclass1,"process_opensles_getPlayData", "([S)I");
    jint len = (jint)env->CallStaticIntMethod(jclass1, mid,jshortArray1);
    LOGI("short len : %d",len);


    jshort * buf = env->GetShortArrayElements(jshortArray1,0);

    if (bqPlayerBufferQueue != NULL){
        LOGI("add data");
        (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, buf, audio_size);
    }
    free(audio_buf);
    env->DeleteLocalRef(jshortArray1);
    env->DeleteLocalRef(jclass1);
}

static void getShortsPlayData(JNIEnv *env, jobject jobj){
    if (env == NULL){
        LOGI("testStatic 1 env == NULL");
        return;
    }

    if (jobj == NULL){
        LOGI("testStatic 1 jobj == NULL");
        return;
    }

    jshort *audio_buf = (jshort*) malloc(audio_size);
    jint audio_size = 1600;

    jclass jclass1 = env->GetObjectClass(jobj);
    jmethodID mid = env->GetStaticMethodID(jclass1,"process_opensles_getPlayShortData", "()[S");
    jshortArray pcmJni = (jshortArray)env->CallStaticObjectMethod(jclass1, mid);
    jint len = 0;
    if (pcmJni != NULL){
        len = env->GetArrayLength(pcmJni);
    }
    LOGI("pcmJni len : %d",len);

    if (bqPlayerBufferQueue != NULL && pcmJni != NULL){
        LOGI("add data");
        jshort *audio_bufP = env->GetShortArrayElements(pcmJni,0);

        (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, audio_bufP, len);
    } else{
        feedData();
    }
    free(audio_buf);
    env->DeleteLocalRef(pcmJni);
    env->DeleteLocalRef(jclass1);
}

static void playerCallback(SLAndroidSimpleBufferQueueItf bq, void *context){
//    LOGI("playerCallback");
    if(playThreadEnv == NULL){
        save_play_Thread_JniEnv();
    }

    getShortsPlayData(playThreadEnv,g_cls);
//    getShortsPlayDataBytes(playThreadEnv,g_cls);
    delete_play_Thread_JniEnv();
}





JNIEXPORT jint JNICALL Java_com_medialib_audio_base_OpenslEs_process_1opensles_1playstart
        (JNIEnv *env, jobject object) {
//    playThreadEnv = env;

    g_cls = env->NewGlobalRef(object);
//    playMethodId = env->GetStaticMethodID(cls, "process_opensles_getPlayDataBytes", "()[B");

//    test(env,object);
//    testStatic(env,object);
//        getShortsPlayDataBytes(playThreadEnv,g_cls);
//        getShortsPlayDataBytes(playThreadEnv,g_cls);
//    testGetString(env,g_cls);

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
    SLDataLocator_AndroidSimpleBufferQueue loc_bufq = { SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 30 };
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



    //录音
    // 参数
    SLDataLocator_IODevice deviceInputLocator = { SL_DATALOCATOR_IODEVICE, SL_IODEVICE_AUDIOINPUT, SL_DEFAULTDEVICEID_AUDIOINPUT, NULL };
    SLDataSource inputSource = { &deviceInputLocator, NULL };

    SLDataLocator_AndroidSimpleBufferQueue inputLocator = { SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 1 };
    SLDataFormat_PCM inputFormat = { SL_DATAFORMAT_PCM, 1, 16000 * 1000, SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16, SL_SPEAKER_FRONT_LEFT , SL_BYTEORDER_LITTLEENDIAN };

    SLDataSink inputSink = { &inputLocator, &inputFormat };

    const SLInterfaceID inputInterfaces[2] = { SL_IID_ANDROIDSIMPLEBUFFERQUEUE, SL_IID_ANDROIDCONFIGURATION };

    const SLboolean requireds[2] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_FALSE };


  /*  // 创建AudioRecorder
    result = (*openSLEngineInterface)->CreateAudioRecorder(openSLEngineInterface, &andioRecorderObject, &inputSource, &inputSink, 2, inputInterfaces, requireds);

// 初始化AudioRecorder
    result = (*andioRecorderObject)->Realize(andioRecorderObject, SL_BOOLEAN_FALSE);

// 获取音频输入的BufferQueue接口
    result = (*andioRecorderObject)->GetInterface(andioRecorderObject, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &inputBufferQueueInterface);

// 获取录制器接口
    SLRecordItf audioRecorderInterface;
    (*andioRecorderObject)->GetInterface(andioRecorderObject, SL_IID_RECORD, &audioRecorderInterface);*/

    //启动
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

//    delete_play_Thread_JniEnv();

    if (g_cls != NULL){
        env->DeleteGlobalRef(g_cls);
    }
    if (playThreadEnv != NULL){
        LOGI("cl_android_get_javaVm");
//        JavaVM* javaVM = cl_android_get_javaVm();
//        javaVM->DetachCurrentThread();
    }
    return 0;
}


JNIEXPORT void JNICALL Java_com_medialib_audio_base_OpenslEs_process_1opensles_1putplaydata
        (JNIEnv *env, jobject jobject1, jshortArray pcmData, jint pcmDataLen){

    jshort * pcmD = env->GetShortArrayElements(pcmData,0);

//    jshort *pcmToPlay = (jshort*)malloc(pcmDataLen);
//    memcpy(pcmToPlay,pcmD, sizeof(jshort));

    if (bqPlayerBufferQueue != NULL){
        (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue, pcmD, pcmDataLen);
    }
//    env->ReleaseShortArrayElements(pcmData,pcmD,0);


//    LOGI("jni  put data");
}