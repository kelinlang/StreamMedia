//
// Created by dengjun on 2019/3/26.
//

#include <library/CommonLib/log/cloudvoice_log.h>
#include <android/native_window_jni.h>
#include <android/log/cloudvoice_android_log.h>
#include <business/model/cloudvoice_player_param.h>
#include <library/CommonLib/utils/cloudvoice_utils.h>
#include "com_stream_media_jni_MediaJni.h"
#include "cloudvoice_media_manager.h"

static CloudVoiceMediaManager mediaManager;


JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
    cloudVoiceAndroidLogInit();
    cloudVoiceLogI("---------------------JNI_OnLoad start------------------------");

    int retval;
    JNIEnv* env = NULL;

    if ((*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_6) != JNI_OK) {
        cloudVoiceLogI("---------------------JNI_OnLoad error------------------------");
        return JNI_ERR;
    }


    //此处可以动态注册本地方法

    cloudVoiceLogI("---------------------JNI_OnLoad finish------------------------");

    return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL Java_com_stream_media_jni_MediaJni_init
        (JNIEnv *env, jobject  object){
    cloudVoiceLogI("media jni init ");
    if(!mediaManager){
        mediaManager = cloudVoiceCreateMediaManager();
    }
    if (mediaManager){
        mediaManager->init(mediaManager);
    }
}

JNIEXPORT void JNICALL Java_com_stream_media_jni_MediaJni_setMediaStatusCallback
        (JNIEnv *, jobject, jobject);


JNIEXPORT void JNICALL Java_com_stream_media_jni_MediaJni_setParam
        (JNIEnv *, jobject, jobject);


JNIEXPORT void JNICALL Java_com_stream_media_jni_MediaJni_release
        (JNIEnv *env, jobject  object){
    cloudVoiceLogI("media jni release ");
    if (mediaManager){
        mediaManager->release(mediaManager);
        mediaManager = NULL;
    }
}



JNIEXPORT void JNICALL Java_com_stream_media_jni_MediaJni_resume
        (JNIEnv *, jobject);


JNIEXPORT void JNICALL Java_com_stream_media_jni_MediaJni_pause
        (JNIEnv *, jobject);


JNIEXPORT void JNICALL Java_com_stream_media_jni_MediaJni_createPlayer
        (JNIEnv *env, jobject object, jstring id){
    cloudVoiceLogI("%s",__FUNCTION__);
    if (mediaManager){
        char* idString = (char*)(*env)->GetStringUTFChars(env,id ,NULL);
        mediaManager->createPlayer(mediaManager,idString);
        (*env)->ReleaseStringUTFChars(env,id, idString);
    }
}

JNIEXPORT void JNICALL Java_com_stream_media_jni_MediaJni_setVideoSurface
        (JNIEnv *env, jobject object, jstring id, jobject surface){
    cloudVoiceLogI("%s",__FUNCTION__);
    if (mediaManager){
        char* idString = (char*)(*env)->GetStringUTFChars(env,id ,NULL);
        ANativeWindow* nativeWindow = ANativeWindow_fromSurface(env, surface);
        mediaManager->setVideoSurface(mediaManager,idString,nativeWindow);
        (*env)->ReleaseStringUTFChars(env,id, idString);
    }
}


JNIEXPORT void JNICALL Java_com_stream_media_jni_MediaJni_setVideoMatrix
        (JNIEnv *env, jobject object, jstring id, jfloatArray matrix){
    cloudVoiceLogI("%s",__FUNCTION__);
    if (mediaManager){
        char* idString = (char*)(*env)->GetStringUTFChars(env,id ,NULL);
        jboolean isCp = JNI_FALSE;
        jfloat * matrixs = (*env)->GetFloatArrayElements(env,matrix, &isCp);//不复制副本
        jint len = (*env)->GetArrayLength(env,matrixs);

        cloudVoiceLogI("matrix len : %d",len);
        float *matrixTmp = (float*)malloc(len);
        memcpy(matrixTmp, matrixs, len);
        mediaManager->setVideoMatrix(mediaManager,idString,matrixTmp);

        (*env)->ReleaseFloatArrayElements(env,matrix,matrixs,0);
        (*env)->ReleaseStringUTFChars(env,id, idString);
    }
}


JNIEXPORT void JNICALL Java_com_stream_media_jni_MediaJni_setPlayerParam
        (JNIEnv *env, jobject object, jstring id, jobject playerParam){
    cloudVoiceLogI("%s",__FUNCTION__);
    if (mediaManager){
        char* idString = (char*)(*env)->GetStringUTFChars(env,id ,NULL);
        jboolean isCp = JNI_FALSE;

        jclass playerParamClass = (*env)->FindClass(env,"com/stream/media/jni/PlayerParam");

        jfieldID  jviewWidth = (*env)->GetFieldID(env,playerParamClass,"viewWidth","I");
        jfieldID  jviewHeight = (*env)->GetFieldID(env,playerParamClass,"viewHeight","I");
        jfieldID  jgravity= (*env)->GetFieldID(env,playerParamClass,"gravity","I");
        jfieldID  jdataFormat= (*env)->GetFieldID(env,playerParamClass,"dataFormat","I");
        jfieldID  jdisplayFormat= (*env)->GetFieldID(env,playerParamClass,"displayFormat","I");
        jfieldID  jvideoWidth= (*env)->GetFieldID(env,playerParamClass,"videoWidth","I");
        jfieldID  jvideoHeight= (*env)->GetFieldID(env,playerParamClass,"videoHeight","I");
        jfieldID  jUrl = (*env)->GetFieldID(env,playerParamClass,"url","Ljava/lang/String;");
        jfieldID  jMatrix = (*env)->GetFieldID(env,playerParamClass,"matrix","[F");

        int viewWidth = (*env)->GetIntField(env,playerParam,jviewWidth);
        int viewHeight = (*env)->GetIntField(env,playerParam,jviewHeight);
        int gravity = (*env)->GetIntField(env,playerParam,jgravity);
        int dataFormat = (*env)->GetIntField(env,playerParam,jdataFormat);
        int displayFormat = (*env)->GetIntField(env,playerParam,jdisplayFormat);
        int videoWidth = (*env)->GetIntField(env,playerParam,jvideoWidth);
        int videoHeight = (*env)->GetIntField(env,playerParam,jvideoHeight);

        jstring url = (jstring)(*env)->GetObjectField(env,playerParam, jUrl);
        jstring * urlString = (*env)->GetStringUTFChars(env,url ,NULL);

        jfloatArray  dataArray = (jfloatArray)(*env)->GetObjectField(env,playerParam,jMatrix);
        jfloatArray* buffer = (jfloatArray*)(*env)->GetFloatArrayElements(env,dataArray, 0);
        int matrixLen = (*env)->GetArrayLength(env,dataArray);
        float* matrix = (uint8_t *) malloc(matrixLen* sizeof(float));//后面再考虑重用内存
        memcpy(matrix,buffer,matrixLen);
        (*env)->ReleaseFloatArrayElements(env,dataArray, buffer, 0);

        CloudVoicePlayerParam playerParam1 = mediaManager->getPlayerParam(mediaManager,idString);
        if (playerParam1){
            playerParam1->viewWidth = viewWidth;
            playerParam1->viewHeight = viewHeight;
            playerParam1->gravity = gravity;
            playerParam1->dataFormat = dataFormat;
            playerParam1->displayFormat = displayFormat;
            playerParam1->videoWidth = videoWidth;
            playerParam1->videoHeight = videoHeight;

            playerParam1->matrixLen = matrixLen;
            playerParam1->matrix = matrix;

            cloudVoiceStringCopy(urlString,&playerParam1->url);
        }

        (*env)->ReleaseStringUTFChars(env,url, urlString);
        (*env)->ReleaseStringUTFChars(env,id, idString);
    }
}


JNIEXPORT void JNICALL Java_com_stream_media_jni_MediaJni_startPlay
        (JNIEnv *env, jobject object, jstring id){
    cloudVoiceLogI("%s",__FUNCTION__);
    if (mediaManager){
        char* idString = (char*)(*env)->GetStringUTFChars(env,id ,NULL);

        mediaManager->startPlay(mediaManager,idString);

        (*env)->ReleaseStringUTFChars(env,id, idString);
    }
}


JNIEXPORT void JNICALL Java_com_stream_media_jni_MediaJni_stopPlay
        (JNIEnv *env, jobject object, jstring id){
    cloudVoiceLogI("%s",__FUNCTION__);
    if (mediaManager){
        char* idString = (char*)(*env)->GetStringUTFChars(env,id ,NULL);
        mediaManager->stopPlay(mediaManager,idString);
        (*env)->ReleaseStringUTFChars(env,id, idString);
    }
}


JNIEXPORT void JNICALL Java_com_stream_media_jni_MediaJni_setPullStreamParam
        (JNIEnv *env, jobject object, jstring id, jobject streamParam){
    if (mediaManager){
        char* idString = (char*)(*env)->GetStringUTFChars(env,id ,NULL);
        jboolean isCp = JNI_FALSE;




        (*env)->ReleaseStringUTFChars(env,id, idString);
    }
}


JNIEXPORT void JNICALL Java_com_stream_media_jni_MediaJni_startPull
        (JNIEnv *env, jobject object, jstring id){
    if (mediaManager){
        char* idString = (char*)(*env)->GetStringUTFChars(env,id ,NULL);
        jboolean isCp = JNI_FALSE;




        (*env)->ReleaseStringUTFChars(env,id, idString);
    }
}

JNIEXPORT void JNICALL Java_com_stream_media_jni_MediaJni_stopPull
        (JNIEnv *env, jobject object, jstring id){
    if (mediaManager){
        char* idString = (char*)(*env)->GetStringUTFChars(env,id ,NULL);
        jboolean isCp = JNI_FALSE;

        (*env)->ReleaseStringUTFChars(env,id, idString);
    }
}


JNIEXPORT void JNICALL Java_com_stream_media_jni_MediaJni_createPushClient
        (JNIEnv *env, jobject object, jstring id){
    cloudVoiceLogI("%s",__FUNCTION__);
    if (mediaManager){
        char* idString = (char*)(*env)->GetStringUTFChars(env,id ,NULL);
        jboolean isCp = JNI_FALSE;

        mediaManager->createPushClient(mediaManager,idString);


        (*env)->ReleaseStringUTFChars(env,id, idString);
    }
}


JNIEXPORT void JNICALL Java_com_stream_media_jni_MediaJni_setPushStreamParam
        (JNIEnv *env, jobject object, jstring id, jobject streamParam){
    cloudVoiceLogI("%s",__FUNCTION__);
    if (mediaManager){
        char* idString = (char*)(*env)->GetStringUTFChars(env,id ,NULL);

        jclass streamParamClass = (*env)->FindClass(env,"com/stream/media/jni/StreamParam");
        jfieldID  jtype = (*env)->GetFieldID(env,streamParamClass,"type","I");
        jfieldID  jUrl = (*env)->GetFieldID(env,streamParamClass,"url","Ljava/lang/String;");

        int type = (*env)->GetIntField(env,streamParam,jtype);


        jstring url = (jstring)(*env)->GetObjectField(env,streamParam, jUrl);
        jstring * urlString = (*env)->GetStringUTFChars(env,url ,NULL);

        CloudVoiceStreamParam streamParam1 = cloudVoiceCreateStreamParam();
        if (streamParam1){
            streamParam1->type = type;
            cloudVoiceStringCopy(urlString,&streamParam1->url);
        }
        mediaManager->setPushStreamParam(mediaManager,idString,streamParam1);

        (*env)->ReleaseStringUTFChars(env,url, urlString);
        (*env)->ReleaseStringUTFChars(env,id, idString);
    }
}


JNIEXPORT void JNICALL Java_com_stream_media_jni_MediaJni_startPush
        (JNIEnv *env, jobject object, jstring id){
    cloudVoiceLogI("%s",__FUNCTION__);
    if (mediaManager){
        char* idString = (char*)(*env)->GetStringUTFChars(env,id ,NULL);

        mediaManager->startPush(mediaManager,idString);

        (*env)->ReleaseStringUTFChars(env,id, idString);
    }
}


JNIEXPORT void JNICALL Java_com_stream_media_jni_MediaJni_stopPush
        (JNIEnv *env, jobject object, jstring id){
    cloudVoiceLogI("%s",__FUNCTION__);
    if (mediaManager){
        char* idString = (char*)(*env)->GetStringUTFChars(env,id ,NULL);

        mediaManager->stopPush(mediaManager,idString);

        (*env)->ReleaseStringUTFChars(env,id, idString);
    }
}


JNIEXPORT void JNICALL Java_com_stream_media_jni_MediaJni_sendVideoData
        (JNIEnv *env, jobject object, jstring id, jobject videoData){
    if (mediaManager){
        char* idString = (char*)(*env)->GetStringUTFChars(env,id ,NULL);
        jboolean isCp = JNI_FALSE;

        jclass videoDataClass = (*env)->FindClass(env,"com/stream/media/jni/VideoData");
        jfieldID  jId = (*env)->GetFieldID(env,videoDataClass,"id","Ljava/lang/String;");
        jfieldID  jDataFormat = (*env)->GetFieldID(env,videoDataClass,"dataFormat","I");
        jfieldID  jwidth = (*env)->GetFieldID(env,videoDataClass,"width","I");
        jfieldID  jheight = (*env)->GetFieldID(env,videoDataClass,"height","I");
        jfieldID  jframeType = (*env)->GetFieldID(env,videoDataClass,"frameType","I");
        jfieldID  jtimeStamp = (*env)->GetFieldID(env,videoDataClass,"timeStamp","J");
        jfieldID  jvideoDataLen = (*env)->GetFieldID(env,videoDataClass,"videoDataLen","I");
        jfieldID  jvideoData = (*env)->GetFieldID(env,videoDataClass,"videoData","[B");

        jfieldID  jspsLen = (*env)->GetFieldID(env,videoDataClass,"spsLen","I");
        jfieldID  jspsData = (*env)->GetFieldID(env,videoDataClass,"sps","[B");
        jfieldID  jppsDataLen = (*env)->GetFieldID(env,videoDataClass,"ppsLen","I");
        jfieldID  jppsData = (*env)->GetFieldID(env,videoDataClass,"pps","[B");


        CloudVoiceAVPacket avPacket = cloudVoiceCreateAVPackect();
        if (avPacket){
            avPacket->packetFormat =  (*env)->GetIntField(env,videoData,jDataFormat);
            if (avPacket->packetFormat == VDIEO_FORMAT_H264){
                jstring id = (jstring)(*env)->GetObjectField(env,videoData, jId);
                char* idString = (char*)(*env)->GetStringUTFChars(env,id ,NULL);
                (*env)->ReleaseStringUTFChars(env,id, idString);

                avPacket->frameType = (*env)->GetIntField(env,videoData,jframeType);
                avPacket->timeStamp = (*env)->GetLongField(env,videoData,jtimeStamp);
                avPacket->dataLen = (*env)->GetIntField(env,videoData,jvideoDataLen);

                jbyteArray dataArray = (jbyteArray)(*env)->GetObjectField(env,videoData,jvideoData);
                jbyte* buffer = (unsigned char *)(*env)->GetByteArrayElements(env,dataArray, 0);
                avPacket->data = (uint8_t *) malloc(avPacket->dataLen);//后面再考虑重用内存
                memcpy(avPacket->data,buffer,avPacket->dataLen);
                (*env)->ReleaseByteArrayElements(env,dataArray, buffer, 0);

            } else if (avPacket->packetFormat == VDIEO_FORMAT_H264_SPS_PPS){
                avPacket->spsLen = (*env)->GetIntField(env,videoData,jspsLen);
                jbyteArray spsdataArray = (jbyteArray)(*env)->GetObjectField(env,videoData,jspsData);
                unsigned char * sps = (unsigned char *)(*env)->GetByteArrayElements(env,spsdataArray, 0);
                avPacket->sps = (uint8_t*) malloc(avPacket->spsLen);
                memcpy(avPacket->sps,sps,avPacket->spsLen);
                (*env)->ReleaseByteArrayElements(env,spsdataArray, (jbyte*)sps, 0);

                avPacket->ppsLen = (*env)->GetIntField(env,videoData,jppsDataLen);
                jbyteArray ppsdataArray = (jbyteArray)(*env)->GetObjectField(env,videoData,jppsData);
                unsigned char * pps = (unsigned char *)(*env)->GetByteArrayElements(env,ppsdataArray, 0);
                avPacket->pps = (uint8_t*) malloc(avPacket->ppsLen);
                memcpy(avPacket->pps,pps,avPacket->ppsLen);
                (*env)->ReleaseByteArrayElements(env,ppsdataArray, (jbyte*)pps, 0);
            }


            mediaManager->sendVideoData(mediaManager,idString,avPacket);
        }

        (*env)->ReleaseStringUTFChars(env,id, idString);
    }
}


JNIEXPORT void JNICALL Java_com_stream_media_jni_MediaJni_sendAudioData
        (JNIEnv *env, jobject object, jstring id, jobject audioData){
    if (mediaManager){
        char* idString = (char*)(*env)->GetStringUTFChars(env,id ,NULL);
        jboolean isCp = JNI_FALSE;

        (*env)->ReleaseStringUTFChars(env,id, idString);
    }
}