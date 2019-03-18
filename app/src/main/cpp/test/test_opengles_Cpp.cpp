//
// Created by dengjun on 2018/12/25.
//

extern "C"{
#include "../StreamMedia/model/sm_video_fourcc.h"
#include "../StreamMedia/model/sm_video_data.h"
#include "../CommonLib/thread/cl_sdl_thread.h"
#include "../StreamMedia/buffer/sm_video_data_buffer.h"
#include "../StreamMedia/display/sm_egl.h"

#include "../CommonLib/log/cl_sdl_log.h"
#include "com_medialib_video_OpenGlEs.h"
#include "../StreamMedia/video/sm_mediacodec_test.h"
#include "RtmpPushClient.h"
#include "RtmpPullClient.h"
}

#include <android/native_window_jni.h>
#include <string.h>

#include "ShaderUtils.h"


#include <EGL/egl.h>

#define GET_STR(x) #x
const char *vertexShaderString = GET_STR(
        attribute vec4 aPosition;
        attribute vec2 aTexCoord;
        varying vec2 vTexCoord;

        uniform         mat4 um4_ModelViewProjection;

        void main() {
            vTexCoord=vec2(aTexCoord.x,1.0-aTexCoord.y);
            gl_Position = um4_ModelViewProjection *aPosition;
        }
);
const char *fragmentShaderString = GET_STR(
        precision mediump float;
        varying vec2 vTexCoord;
        uniform sampler2D yTexture;
        uniform sampler2D uTexture;
        uniform sampler2D vTexture;
        void main() {
            vec3 yuv;
            vec3 rgb;
            yuv.r = texture2D(yTexture, vTexCoord).r;
            yuv.g = texture2D(uTexture, vTexCoord).r - 0.5;
            yuv.b = texture2D(vTexture, vTexCoord).r - 0.5;
            rgb = mat3(1.164,  1.164,  1.164,
                       0.0,   -0.213,  2.112,
                       1.793, -0.533,  0.0) * yuv;
            gl_FragColor = vec4(rgb, 1.0);
        }
);

ClThread displayThread;
int displayThreadRunFlag = 0;
SmVideoDataQueue videoDataQueue;

SmVideoParam videoParam;
ANativeWindow* mANativeWindow;

float* cacheMatrixTmp;


SmMediaCodec  mediaCodec;

SmRtmpPushClient rtmpPushClient;
SmRtmpPullClient rtmpPullClient;


static int videoDisplayThread(void *arg){
    LOGI("videoDisplayThread 0");

    /**
   *初始化egl
   **/
    EGLConfig eglConf;
    EGLSurface eglWindow;
    EGLContext eglCtx;
    int windowWidth;
    int windowHeight;
//    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);

    EGLint configSpec[] = { EGL_RED_SIZE, 8,
                            EGL_GREEN_SIZE, 8,
                            EGL_BLUE_SIZE, 8,
                            EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_NONE };

    EGLDisplay eglDisp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    EGLint eglMajVers, eglMinVers;
    EGLint numConfigs;
    eglInitialize(eglDisp, &eglMajVers, &eglMinVers);
    eglChooseConfig(eglDisp, configSpec, &eglConf, 1, &numConfigs);

    eglWindow = eglCreateWindowSurface(eglDisp, eglConf,mANativeWindow, NULL);

    eglQuerySurface(eglDisp,eglWindow,EGL_WIDTH,&windowWidth);
    eglQuerySurface(eglDisp,eglWindow,EGL_HEIGHT,&windowHeight);
    const EGLint ctxAttr[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
    };
    eglCtx = eglCreateContext(eglDisp, eglConf,EGL_NO_CONTEXT, ctxAttr);

    eglMakeCurrent(eglDisp, eglWindow, eglWindow, eglCtx);


    /**
     * 设置opengl 要在egl初始化后进行
     * **/
    float *vertexData = new float[12]{
            1.0f, -1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f
    };

    float *textureVertexData = new float[8]{
            1.0f, 0.0f,//右下
            0.0f, 0.0f,//左下
            1.0f, 1.0f,//右上
            0.0f, 1.0f//左上
    };

    const GLfloat g_bt709[] = {
            1.164,  1.164,  1.164,
            0.0,   -0.213,  2.112,
            1.793, -0.533,  0.0,
    };

    ShaderUtils *shaderUtils = new ShaderUtils();

    GLuint programId = shaderUtils->createProgram(vertexShaderString,fragmentShaderString );
    delete shaderUtils;
    GLuint aPositionHandle = (GLuint) glGetAttribLocation(programId, "aPosition");
    GLuint aTextureCoordHandle = (GLuint) glGetAttribLocation(programId, "aTexCoord");

    GLuint textureSamplerHandleY = (GLuint) glGetUniformLocation(programId, "yTexture");
    GLuint textureSamplerHandleU = (GLuint) glGetUniformLocation(programId, "uTexture");
    GLuint textureSamplerHandleV = (GLuint) glGetUniformLocation(programId, "vTexture");
    GLuint um4_mvp = glGetUniformLocation(programId,"um4_ModelViewProjection");


    //因为没有用矩阵所以就手动自适应
    int videoWidth = 640;
    int videoHeight = 480;

    int left,top,viewWidth,viewHeight;
    if(windowHeight > windowWidth){
        left = 0;
        viewWidth = windowWidth;
        viewHeight = (int)(videoHeight*1.0f/videoWidth*viewWidth);
        top = (windowHeight - viewHeight)/2;
    }else{
        top = 0;
        viewHeight = windowHeight;
        viewWidth = (int)(videoWidth*1.0f/videoHeight*viewHeight);
        left = (windowWidth - viewWidth)/2;
    }
    glViewport(left, top, viewWidth, viewHeight);

    glUseProgram(programId);
    glEnableVertexAttribArray(aPositionHandle);
    glVertexAttribPointer(aPositionHandle, 3, GL_FLOAT, GL_FALSE,
                          12, vertexData);
    glEnableVertexAttribArray(aTextureCoordHandle);
    glVertexAttribPointer(aTextureCoordHandle,2,GL_FLOAT,GL_FALSE,8,textureVertexData);
    /***
     * 初始化空的yuv纹理
     * **/
    GLuint yTextureId;
    GLuint uTextureId;
    GLuint vTextureId;
    glGenTextures(1,&yTextureId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,yTextureId);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glUniform1i(textureSamplerHandleY,0);

    glGenTextures(1,&uTextureId);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,uTextureId);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glUniform1i(textureSamplerHandleU,1);

    glGenTextures(1,&vTextureId);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D,vTextureId);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glUniform1i(textureSamplerHandleV,2);

//    glUniformMatrix3fv(um4_mvp, 1, GL_FALSE, SM_GLES2_getColorMatrix_bt709());
    glUniformMatrix4fv(um4_mvp, 1, GL_FALSE, cacheMatrixTmp);

    /***
     * 开始解码
     * **/
    int ret;
    while (displayThreadRunFlag) {
        SmVideoDataNode videoDataNode = smVideoDataQueueDequeueData(videoDataQueue);
        if(videoDataNode){
//                    LOGI("display loop ------------ 2");
            SmVideoData videoData = videoDataNode->videoData;

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, yTextureId);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, videoData->width, videoData->height,0, GL_LUMINANCE, GL_UNSIGNED_BYTE, videoData->pixelsY);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, uTextureId);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,  videoData->width/2, videoData->height/2,0, GL_LUMINANCE, GL_UNSIGNED_BYTE, videoData->pixelsU);

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, vTextureId);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,  videoData->width/2, videoData->height/2,0, GL_LUMINANCE, GL_UNSIGNED_BYTE, videoData->pixelsV);


            /***
            * 纹理更新完成后开始绘制
            ***/
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            eglSwapBuffers(eglDisp, eglWindow);

            smCacheVideoDataNodeToCache(videoDataQueue,videoDataNode);
        }
    }
    /***
     * 释放资源
     * **/
    delete vertexData;
    delete textureVertexData;

    eglMakeCurrent(eglDisp, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(eglDisp, eglCtx);
    eglDestroySurface(eglDisp, eglWindow);
    eglTerminate(eglDisp);
    eglDisp = EGL_NO_DISPLAY;
    eglWindow = EGL_NO_SURFACE;
    eglCtx = EGL_NO_CONTEXT;
}


static void yuvDataToQueue(uint8_t* yuvJniData){
    if(videoDataQueue) {
        SmVideoDataNode videoDataNode = smCreateVideoDataNodeFromCache(videoDataQueue);
        if (videoDataNode) {
//            LOGI("sendYuvData  ------------ 1");

            SmVideoData videoData = videoDataNode->videoData;
            videoData->dataFormat = SM_VIDEO_FCC_I420;
            videoData->pitches[0] = videoParam->viewWidth * videoParam->viewHeight;
            videoData->pitches[1] = videoParam->viewWidth * videoParam->viewHeight / 4;
            videoData->pitches[2] = videoParam->viewWidth * videoParam->viewHeight / 4;
            videoData->width = videoParam->viewWidth;
            videoData->height = videoParam->viewHeight;

            if (videoData->pixelsY == NULL) {
                //如果没有存储空间，则分配
                videoData->pixelsY = (unsigned char *) malloc(videoData->pitches[0]);
            }

            if (videoData->pixelsU == NULL) {
                //如果没有存储空间，则分配
                videoData->pixelsU = (unsigned char *) malloc(videoData->pitches[1]);
            }

            if (videoData->pixelsV == NULL) {
                //如果没有存储空间，则分配
                videoData->pixelsV = (unsigned char *) malloc(videoData->pitches[2]);
            }

            //复制数据
            memcpy(videoData->pixelsY, yuvJniData, videoData->pitches[0]);
            memcpy(videoData->pixelsU, yuvJniData + videoData->pitches[0], videoData->pitches[1]);
            memcpy(videoData->pixelsV, yuvJniData + videoData->pitches[0] + videoData->pitches[1],
                   videoData->pitches[2]);

//            LOGI("sendYuvData  ------------ 5");
            //数据放到队列
            smVideoDataQueueEnqueueData(videoDataQueue, videoDataNode);
//            LOGI("sendYuvData  ------------ 6");
        }
    }
}

static void yuvDataCallback(int type,  int dataSize,void * data, void * ext){
    uint8_t* yuvJniData = (uint8_t*)data;

    yuvDataToQueue(yuvJniData);
}

static void h264DataCallback(int type,  int dataSize,void * data, void * ext){
    smMediaCodecAddData(mediaCodec,(uint8_t*)data,dataSize);
}

JNIEXPORT jint JNICALL Java_com_medialib_video_OpenGlEs_setSurface
        (JNIEnv * env, jobject object, jobject surface){
    mANativeWindow = ANativeWindow_fromSurface(env, surface);
    return 0;
}

JNIEXPORT jint JNICALL Java_com_medialib_video_OpenGlEs_setMatrix
        (JNIEnv * env, jobject object, jfloatArray matrix){
    cacheMatrixTmp = (float*)malloc(16* sizeof(float));

    float * matrixs = (float *)env->GetFloatArrayElements(matrix, 0);
    memcpy(cacheMatrixTmp, matrixs, 16);
    env->ReleaseFloatArrayElements(matrix,(jfloat *)matrixs,0);
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

    mediaCodec = smCreateMediaCodec();
    smMediaCodecSetDataCallback(mediaCodec,yuvDataCallback);
    smMediaCodecstart(mediaCodec);

    rtmpPushClient = smCreateRtmpPushClient();
    smRtmpPushClientStart(rtmpPushClient);

    rtmpPullClient = smCreateRtmpPullClient();
    smRtmpPullClientSetDataCallback(rtmpPullClient,h264DataCallback);
    smRtmpPullClientStart(rtmpPullClient);
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
    smMediaCodecRelease(mediaCodec);
    mediaCodec = NULL;

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

    if(rtmpPushClient){
        smRtmpPushClientStop(rtmpPushClient);
        free(rtmpPushClient);
        rtmpPushClient = NULL;
    }
    if (rtmpPullClient){
        smRtmpPullClientStop(rtmpPullClient);
        free(rtmpPullClient);
        rtmpPullClient = NULL;
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
    unsigned char * yuvJniData = (unsigned char *)env->GetByteArrayElements(yuvData, 0);

    yuvDataToQueue(yuvJniData);

    env->ReleaseByteArrayElements(yuvData,(jbyte*)yuvJniData,0);
}

JNIEXPORT void JNICALL Java_com_medialib_video_OpenGlEs_sendData
        (JNIEnv * env, jobject object, jint type, jbyteArray data, jint dataLen){
    unsigned char * h264Data = (unsigned char *)env->GetByteArrayElements(data, 0);
//    LOGI("sendData  1----------------------------------------");
    smMediaCodecAddData(mediaCodec,h264Data,dataLen);

    env->ReleaseByteArrayElements(data,(jbyte*)h264Data,0);
}

JNIEXPORT void JNICALL Java_com_medialib_video_OpenGlEs_sendVideoData
        (JNIEnv *env, jobject object, jobject videoData){
    if (rtmpPushClient == NULL){
        return;
    }

    jclass videoDataClass = env->FindClass("com/stream/media/demo/camera/VideoData");
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


    SmVideoDataNode videoDataNode = smRtmpPushClientGetCacheVideoData(rtmpPushClient);
    if (videoDataNode && videoDataNode->videoData){
        SmVideoData smVideoData = videoDataNode->videoData;

        LOGD("debug -------------------------  0");
        smVideoData->dataFormat = env->GetIntField(videoData,jDataFormat);

        if (smVideoData->dataFormat == VDIEO_FORMAT_H264){
            jstring id = (jstring)env->GetObjectField(videoData, jId);
            char* idString = (char*)env->GetStringUTFChars(id ,NULL);
            strncpy(smVideoData->id,idString,SM_ID_MAX_LEN);
            env->ReleaseStringUTFChars(id, idString);

            LOGD("debug -------------------------  1");

            smVideoData->width = env->GetIntField(videoData,jwidth);
            smVideoData->height = env->GetIntField(videoData,jheight);
            smVideoData->frameType = env->GetIntField(videoData,jframeType);
            smVideoData->timeStamp = env->GetLongField(videoData,jtimeStamp);
            smVideoData->pixelsDataLen = env->GetIntField(videoData,jvideoDataLen);

            LOGD("debug -------------------------  2");

            jbyteArray dataArray = (jbyteArray)env->GetObjectField(videoData,jvideoData);
            unsigned char * buffer = (unsigned char *)env->GetByteArrayElements(dataArray, 0);
            if(!smVideoData->pixelsData){
                smVideoData->pixelsData = (uint8_t *) malloc(smVideoData->pixelsDataLen);//后面再考虑重用内存
            }
            memcpy(smVideoData->pixelsData,buffer,smVideoData->pixelsDataLen);
            env->ReleaseByteArrayElements(dataArray, (jbyte*)buffer, 0);

            LOGD("debug -------------------------  3");
        } else if(smVideoData->dataFormat == VDIEO_FORMAT_H264_SPS_PPS){
            smVideoData->spsLen = env->GetIntField(videoData,jspsLen);
            jbyteArray spsdataArray = (jbyteArray)env->GetObjectField(videoData,jspsData);
            unsigned char * sps = (unsigned char *)env->GetByteArrayElements(spsdataArray, 0);
            smVideoData->sps = (uint8_t*) malloc(smVideoData->spsLen);
            memcpy(smVideoData->sps,sps,smVideoData->spsLen);
            env->ReleaseByteArrayElements(spsdataArray, (jbyte*)sps, 0);

            smVideoData->ppsLen = env->GetIntField(videoData,jppsDataLen);
            jbyteArray ppsdataArray = (jbyteArray)env->GetObjectField(videoData,jppsData);
            unsigned char * pps = (unsigned char *)env->GetByteArrayElements(ppsdataArray, 0);
            smVideoData->pps = (uint8_t*) malloc(smVideoData->ppsLen);
            memcpy(smVideoData->pps,pps,smVideoData->ppsLen);
            env->ReleaseByteArrayElements(ppsdataArray, (jbyte*)pps, 0);
        }

        LOGD("debug -------------------------  4");
        smRtmpPushClientAddData(rtmpPushClient,videoDataNode);
    } else{
        LOGD("debug -------------------------error   1");
    }
}