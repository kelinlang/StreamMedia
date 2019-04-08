//
// Created by dengjun on 2019/3/26.
//
#include "com_stream_media_jni_MediaJni.h"
#include "cloudvoice_media_manager_cpp.h"
extern "C" {
#include <library/CommonLib/log/cloudvoice_log.h>
#include <android/log/cloudvoice_android_log.h>
#include <business/model/cloudvoice_player_param.h>
#include <library/CommonLib/utils/cloudvoice_utils.h>


}

#include <android/native_window_jni.h>



static CloudVoiceMediaManager mediaManager;


JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
    cloudVoiceAndroidLogInit();
    cloudVoiceLogI("---------------------JNI_OnLoad start------------------------");

    int retval;
    JNIEnv* env = NULL;

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK) {
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
        char* idString = (char*)env->GetStringUTFChars(id ,NULL);
        mediaManager->createPlayer(mediaManager,idString);
        env->ReleaseStringUTFChars(id, idString);
    }
}

JNIEXPORT void JNICALL Java_com_stream_media_jni_MediaJni_setVideoSurface
        (JNIEnv *env, jobject object, jstring id, jobject surface){
    cloudVoiceLogI("%s",__FUNCTION__);
    if (mediaManager){
        char* idString = (char*)env->GetStringUTFChars(id ,NULL);
        ANativeWindow* nativeWindow = ANativeWindow_fromSurface(env,surface);
        mediaManager->setVideoSurface(mediaManager,idString,nativeWindow);
        env->ReleaseStringUTFChars(id, idString);
    }
}


JNIEXPORT void JNICALL Java_com_stream_media_jni_MediaJni_setVideoMatrix
        (JNIEnv *env, jobject object, jstring id, jfloatArray matrix){
    cloudVoiceLogI("%s",__FUNCTION__);
    if (mediaManager){
        char* idString = (char*)env->GetStringUTFChars(id ,NULL);
        jboolean isCp = JNI_FALSE;
        jfloat * matrixs = env->GetFloatArrayElements(matrix, &isCp);//不复制副本
        jint len = env->GetArrayLength(matrix);

        cloudVoiceLogI("matrix len : %d",len);
        float *matrixTmp = (float*)malloc(len);
        memcpy(matrixTmp, matrixs, len);
        mediaManager->setVideoMatrix(mediaManager,idString,matrixTmp);

        env->ReleaseFloatArrayElements(matrix,matrixs,0);
        env->ReleaseStringUTFChars(id, idString);
    }
}


JNIEXPORT void JNICALL Java_com_stream_media_jni_MediaJni_setPlayerParam
        (JNIEnv *env, jobject object, jstring id, jobject playerParam){
    cloudVoiceLogI("%s",__FUNCTION__);
    if (mediaManager){
        char* idString = (char*)env->GetStringUTFChars(id ,NULL);
        jboolean isCp = JNI_FALSE;

        jclass playerParamClass = env->FindClass("com/stream/media/jni/PlayerParam");

        jfieldID  jviewWidth = env->GetFieldID(playerParamClass,"viewWidth","I");
        jfieldID  jviewHeight = env->GetFieldID(playerParamClass,"viewHeight","I");
        jfieldID  jgravity= env->GetFieldID(playerParamClass,"gravity","I");
        jfieldID  jdataFormat= env->GetFieldID(playerParamClass,"dataFormat","I");
        jfieldID  jdisplayFormat= env->GetFieldID(playerParamClass,"displayFormat","I");
        jfieldID  jvideoWidth= env->GetFieldID(playerParamClass,"videoWidth","I");
        jfieldID  jvideoHeight= env->GetFieldID(playerParamClass,"videoHeight","I");
        jfieldID  jUrl = env->GetFieldID(playerParamClass,"url","Ljava/lang/String;");
        jfieldID  jMatrix = env->GetFieldID(playerParamClass,"matrix","[F");

        int viewWidth = env->GetIntField(playerParam,jviewWidth);
        int viewHeight = env->GetIntField(playerParam,jviewHeight);
        int gravity = env->GetIntField(playerParam,jgravity);
        int dataFormat = env->GetIntField(playerParam,jdataFormat);
        int displayFormat = env->GetIntField(playerParam,jdisplayFormat);
        int videoWidth = env->GetIntField(playerParam,jvideoWidth);
        int videoHeight = env->GetIntField(playerParam,jvideoHeight);

        jstring url = (jstring)env->GetObjectField(playerParam, jUrl);
        char * urlString = (char*)env->GetStringUTFChars(url ,NULL);

        jfloatArray  dataArray = (jfloatArray)env->GetObjectField(playerParam,jMatrix);
        jfloat* buffer = (jfloat *)env->GetFloatArrayElements(dataArray, 0);
        int matrixLen = env->GetArrayLength(dataArray);
        float* matrix = (float *) malloc(matrixLen* sizeof(float));//后面再考虑重用内存
        memcpy(matrix,buffer,matrixLen);
        env->ReleaseFloatArrayElements(dataArray, buffer, 0);

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

            cloudVoiceStringCopy((char*)urlString,&playerParam1->url);
        }

        env->ReleaseStringUTFChars(url, urlString);
        env->ReleaseStringUTFChars(id, idString);
    }
}


JNIEXPORT void JNICALL Java_com_stream_media_jni_MediaJni_startPlay
        (JNIEnv *env, jobject object, jstring id){
    cloudVoiceLogI("%s",__FUNCTION__);
    if (mediaManager){
        char* idString = (char*)env->GetStringUTFChars(id ,NULL);

        mediaManager->startPlay(mediaManager,idString);

        env->ReleaseStringUTFChars(id, idString);
    }
}


JNIEXPORT void JNICALL Java_com_stream_media_jni_MediaJni_stopPlay
        (JNIEnv *env, jobject object, jstring id){
    cloudVoiceLogI("%s",__FUNCTION__);
    if (mediaManager){
        char* idString = (char*)env->GetStringUTFChars(id ,NULL);
        mediaManager->stopPlay(mediaManager,idString);
        env->ReleaseStringUTFChars(id, idString);
    }
}


JNIEXPORT void JNICALL Java_com_stream_media_jni_MediaJni_setPullStreamParam
        (JNIEnv *env, jobject object, jstring id, jobject streamParam){
    if (mediaManager){
        char* idString = (char*)env->GetStringUTFChars(id ,NULL);
        jboolean isCp = JNI_FALSE;




        env->ReleaseStringUTFChars(id, idString);
    }
}


JNIEXPORT void JNICALL Java_com_stream_media_jni_MediaJni_startPull
        (JNIEnv *env, jobject object, jstring id){
    if (mediaManager){
        char* idString = (char*)env->GetStringUTFChars(id ,NULL);
        jboolean isCp = JNI_FALSE;




        env->ReleaseStringUTFChars(id, idString);
    }
}

JNIEXPORT void JNICALL Java_com_stream_media_jni_MediaJni_stopPull
        (JNIEnv *env, jobject object, jstring id){
    if (mediaManager){
        char* idString = (char*)env->GetStringUTFChars(id ,NULL);
        jboolean isCp = JNI_FALSE;

        env->ReleaseStringUTFChars(id, idString);
    }
}


JNIEXPORT void JNICALL Java_com_stream_media_jni_MediaJni_createPushClient
        (JNIEnv *env, jobject object, jstring id){
    cloudVoiceLogI("%s",__FUNCTION__);
    if (mediaManager){
        char* idString = (char*)env->GetStringUTFChars(id ,NULL);
        jboolean isCp = JNI_FALSE;

        mediaManager->createPushClient(mediaManager,idString);


        env->ReleaseStringUTFChars(id, idString);
    }
}


JNIEXPORT void JNICALL Java_com_stream_media_jni_MediaJni_setPushStreamParam
        (JNIEnv *env, jobject object, jstring id, jobject streamParam){
    cloudVoiceLogI("%s",__FUNCTION__);
    if (mediaManager){
        char* idString = (char*)env->GetStringUTFChars(id ,NULL);

        jclass streamParamClass = env->FindClass("com/stream/media/jni/StreamParam");
        jfieldID  jtype = env->GetFieldID(streamParamClass,"type","I");
        jfieldID  jUrl = env->GetFieldID(streamParamClass,"url","Ljava/lang/String;");

        int type = env->GetIntField(streamParam,jtype);


        jstring url = (jstring)env->GetObjectField(streamParam, jUrl);
        char *urlString = (char*)env->GetStringUTFChars(url ,NULL);

        CloudVoiceStreamParam streamParam1 = cloudVoiceCreateStreamParam();
        if (streamParam1){
            streamParam1->type = type;
            cloudVoiceStringCopy(urlString,&streamParam1->url);
        }
        mediaManager->setPushStreamParam(mediaManager,idString,streamParam1);

        env->ReleaseStringUTFChars(url, urlString);
        env->ReleaseStringUTFChars(id, idString);
    }
}


JNIEXPORT void JNICALL Java_com_stream_media_jni_MediaJni_startPush
        (JNIEnv *env, jobject object, jstring id){
    cloudVoiceLogI("%s",__FUNCTION__);
    if (mediaManager){
        char* idString = (char*)env->GetStringUTFChars(id ,NULL);

        mediaManager->startPush(mediaManager,idString);

        env->ReleaseStringUTFChars(id, idString);
    }
}


JNIEXPORT void JNICALL Java_com_stream_media_jni_MediaJni_stopPush
        (JNIEnv *env, jobject object, jstring id){
    cloudVoiceLogI("%s",__FUNCTION__);
    if (mediaManager){
        char* idString = (char*)env->GetStringUTFChars(id ,NULL);

        mediaManager->stopPush(mediaManager,idString);

        env->ReleaseStringUTFChars(id, idString);
    }
}


JNIEXPORT void JNICALL Java_com_stream_media_jni_MediaJni_sendVideoData
        (JNIEnv *env, jobject object, jstring id, jobject videoData){
    if (mediaManager){
        char* idString = (char*)env->GetStringUTFChars(id ,NULL);
        jboolean isCp = JNI_FALSE;

        jclass videoDataClass = env->FindClass("com/stream/media/jni/VideoData");
        jfieldID  jId = env->GetFieldID(videoDataClass,"id","Ljava/lang/String;");
        jfieldID  jDataFormat = env->GetFieldID(videoDataClass,"dataFormat","I");
        jfieldID  jwidth = env->GetFieldID(videoDataClass,"width","I");
        jfieldID  jheight = env->GetFieldID(videoDataClass,"height","I");
        jfieldID  jframeType = env->GetFieldID(videoDataClass,"frameType","I");
        jfieldID  jtimeStamp = env->GetFieldID(videoDataClass,"timeStamp","J");
        jfieldID  jvideoDataLen = env->GetFieldID(videoDataClass,"videoDataLen","I");
        jfieldID  jvideoData = env->GetFieldID(videoDataClass,"videoData","[B");

        jfieldID  jspsLen = env->GetFieldID(videoDataClass,"spsLen","I");
        jfieldID  jspsData = env->GetFieldID(videoDataClass,"sps","[B");
        jfieldID  jppsDataLen = env->GetFieldID(videoDataClass,"ppsLen","I");
        jfieldID  jppsData = env->GetFieldID(videoDataClass,"pps","[B");


        CloudVoiceAVPacket avPacket = cloudVoiceCreateAVPackect();
        if (avPacket){
            avPacket->packetFormat =  env->GetIntField(videoData,jDataFormat);
            if (avPacket->packetFormat == VDIEO_FORMAT_H264){
                jstring id = (jstring)env->GetObjectField(videoData, jId);
                char* idString = (char*)env->GetStringUTFChars(id ,NULL);
                env->ReleaseStringUTFChars(id, idString);

                avPacket->frameType = env->GetIntField(videoData,jframeType);
                avPacket->timeStamp = env->GetLongField(videoData,jtimeStamp);
                avPacket->dataLen = env->GetIntField(videoData,jvideoDataLen);

                jbyteArray dataArray = (jbyteArray)env->GetObjectField(videoData,jvideoData);
                jbyte* buffer = env->GetByteArrayElements(dataArray, 0);
                avPacket->data = (uint8_t *) malloc(avPacket->dataLen);//后面再考虑重用内存
                memcpy(avPacket->data,buffer,avPacket->dataLen);
                env->ReleaseByteArrayElements(dataArray, buffer, 0);

            } else if (avPacket->packetFormat == VDIEO_FORMAT_H264_SPS_PPS){
                avPacket->spsLen = env->GetIntField(videoData,jspsLen);
                jbyteArray spsdataArray = (jbyteArray)env->GetObjectField(videoData,jspsData);
                unsigned char * sps = (unsigned char *)env->GetByteArrayElements(spsdataArray, 0);
                avPacket->sps = (uint8_t*) malloc(avPacket->spsLen);
                memcpy(avPacket->sps,sps,avPacket->spsLen);
                env->ReleaseByteArrayElements(spsdataArray, (jbyte*)sps, 0);

                avPacket->ppsLen = env->GetIntField(videoData,jppsDataLen);
                jbyteArray ppsdataArray = (jbyteArray)env->GetObjectField(videoData,jppsData);
                unsigned char * pps = (unsigned char *)env->GetByteArrayElements(ppsdataArray, 0);
                avPacket->pps = (uint8_t*) malloc(avPacket->ppsLen);
                memcpy(avPacket->pps,pps,avPacket->ppsLen);
                env->ReleaseByteArrayElements(ppsdataArray, (jbyte*)pps, 0);
            }


            mediaManager->sendVideoData(mediaManager,idString,avPacket);
        }

        env->ReleaseStringUTFChars(id, idString);
    }
}


JNIEXPORT void JNICALL Java_com_stream_media_jni_MediaJni_sendAudioData
        (JNIEnv *env, jobject object, jstring id, jobject audioData){
    if (mediaManager){
        char* idString = (char*)env->GetStringUTFChars(id ,NULL);
        jboolean isCp = JNI_FALSE;

        env->ReleaseStringUTFChars(id, idString);
    }
}