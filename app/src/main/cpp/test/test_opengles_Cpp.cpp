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
        void main() {
            vTexCoord=vec2(aTexCoord.x,1.0-aTexCoord.y);
            gl_Position = aPosition;
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
            rgb = mat3(1.0,       1.0,         1.0,
                       0.0,       -0.39465,  2.03211,
                       1.13983, -0.58060,  0.0) * yuv;
            gl_FragColor = vec4(rgb, 1.0);
        }
);

ClThread displayThread;
int displayThreadRunFlag = 0;
SmVideoDataQueue videoDataQueue;

SmVideoParam videoParam;
ANativeWindow* mANativeWindow;

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
    ShaderUtils *shaderUtils = new ShaderUtils();

    GLuint programId = shaderUtils->createProgram(vertexShaderString,fragmentShaderString );
    delete shaderUtils;
    GLuint aPositionHandle = (GLuint) glGetAttribLocation(programId, "aPosition");
    GLuint aTextureCoordHandle = (GLuint) glGetAttribLocation(programId, "aTexCoord");

    GLuint textureSamplerHandleY = (GLuint) glGetUniformLocation(programId, "yTexture");
    GLuint textureSamplerHandleU = (GLuint) glGetUniformLocation(programId, "uTexture");
    GLuint textureSamplerHandleV = (GLuint) glGetUniformLocation(programId, "vTexture");



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


JNIEXPORT jint JNICALL Java_com_medialib_video_OpenGlEs_setSurface
        (JNIEnv * env, jobject object, jobject surface){
    mANativeWindow = ANativeWindow_fromSurface(env, surface);
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

    if(videoDataQueue) {
        SmVideoDataNode videoDataNode = smCreateVideoDataNodeFromCache(videoDataQueue);
        if(videoDataNode){
//            LOGI("sendYuvData  ------------ 1");

            SmVideoData  videoData = videoDataNode->videoData;
            videoData->dataFormat = SM_VIDEO_FCC_I420;
            videoData->pitches[0] = videoParam->viewWidth*videoParam->viewHeight;
            videoData->pitches[1] = videoParam->viewWidth*videoParam->viewHeight/4;
            videoData->pitches[2] = videoParam->viewWidth*videoParam->viewHeight/4;
            videoData->width = videoParam->viewWidth;
            videoData->height = videoParam->viewHeight;

            if(videoData->pixelsY == NULL){
                //如果没有存储空间，则分配
                videoData->pixelsY = (unsigned char*)malloc(videoData->pitches[0]);
            }

            if(videoData->pixelsU == NULL){
                //如果没有存储空间，则分配
                videoData->pixelsU = (unsigned char*)malloc(videoData->pitches[1]);
            }

            if(videoData->pixelsV== NULL){
                //如果没有存储空间，则分配
                videoData->pixelsV = (unsigned char*)malloc(videoData->pitches[2]);
            }

            //复制数据
            memcpy(videoData->pixelsY, yuvJniData, videoData->pitches[0]);
            memcpy(videoData->pixelsU, yuvJniData+videoData->pitches[0], videoData->pitches[1]);
            memcpy(videoData->pixelsV, yuvJniData+videoData->pitches[0]+videoData->pitches[1], videoData->pitches[2]);

//            LOGI("sendYuvData  ------------ 5");
            //数据放到队列
            smVideoDataQueueEnqueueData(videoDataQueue,videoDataNode);
//            LOGI("sendYuvData  ------------ 6");
        }
    }

    env->ReleaseByteArrayElements(yuvData,(jbyte*)yuvJniData,0);
}