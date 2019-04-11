//
// Created by dengjun on 2019/3/22.
//

#include <sys/types.h>
#include <EGL/egl.h>
#include <malloc.h>
#include <pthread.h>
#include <android/native_window_jni.h>
#include <GLES2/gl2.h>
#include <memory.h>


#include <library/CommonLib/queue/cloudvoice_blocking_queue.h>
#include <library/CommonLib/log/cloudvoice_log.h>

#include "cloudvoice_android_video_display.h"
#include "cloudvoice_shader_utils.h"


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

#define SM_GLES2_MAX_PLANE 3

typedef  struct CloudVoiceDisplayOpaque_{
    CloudVoiceDisplayParam displayParam;
    int workFlag;
    pthread_t workThreadId;

    CloudVoiceBlockingQueue blockingQueue;


    MediaStatusCallback mediaStatusCallback;
    MediaDataCallback mediaDataCallback;



    //opengl相关
    float* cacheMatrixTmp;
    ANativeWindow *aNativeWindow;

    int pitches[SM_GLES2_MAX_PLANE]; /**< in bytes, Read-only */  //像素长度
    unsigned char* pixelsY;
    unsigned char* pixelsU;
    unsigned char* pixelsV;

}CloudVoiceDisplayOpaque_;


static void yuvDataChange(CloudVoiceDisplay display, CloudVoiceAVPacket avPackect){
    CloudVoiceDisplayOpaque displayOpaque = display->displayOpaque;
    CloudVoiceDisplayParam displayParam = displayOpaque->displayParam;

    displayOpaque->pitches[0] = displayParam->videoWidth * displayParam->videoHeight;
    displayOpaque->pitches[1] = displayParam->videoWidth * displayParam->videoHeight / 4;
    displayOpaque->pitches[2] = displayParam->videoWidth * displayParam->videoHeight / 4;


    if (displayOpaque->pixelsY == NULL) {
        //如果没有存储空间，则分配
        displayOpaque->pixelsY = (unsigned char *) malloc(displayOpaque->pitches[0]);
    }

    if (displayOpaque->pixelsU == NULL) {
        //如果没有存储空间，则分配
        displayOpaque->pixelsU = (unsigned char *) malloc(displayOpaque->pitches[1]);
    }

    if (displayOpaque->pixelsV == NULL) {
        //如果没有存储空间，则分配
        displayOpaque->pixelsV = (unsigned char *) malloc(displayOpaque->pitches[2]);
    }

    //复制数据
    memcpy(displayOpaque->pixelsY, avPackect->data, displayOpaque->pitches[0]);
    memcpy(displayOpaque->pixelsU, avPackect->data + displayOpaque->pitches[0], displayOpaque->pitches[1]);
    memcpy(displayOpaque->pixelsV, avPackect->data + displayOpaque->pitches[0] + displayOpaque->pitches[1], displayOpaque->pitches[2]);
}


static void* workThreadFunc(void *arg){
    cloudVoiceLogD("display 1");
    CloudVoiceDisplay display = (CloudVoiceDisplay)arg;
    CloudVoiceDisplayOpaque displayOpaque = display->displayOpaque;
    CloudVoiceDisplayParam displayParam = displayOpaque->displayParam;

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

    eglWindow = eglCreateWindowSurface(eglDisp, eglConf,displayOpaque->aNativeWindow, NULL);

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
    float vertexData[] = {
        1.0f, -1.0f, 0.0f,
                -1.0f, -1.0f, 0.0f,
                1.0f, 1.0f, 0.0f,
                -1.0f, 1.0f, 0.0f
    };

    float textureVertexData[] ={
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

//    ShaderUtils *shaderUtils = new ShaderUtils();

    GLuint programId = cloudVoiceCreateProgram(vertexShaderString,fragmentShaderString );

    GLuint aPositionHandle = (GLuint) glGetAttribLocation(programId, "aPosition");
    GLuint aTextureCoordHandle = (GLuint) glGetAttribLocation(programId, "aTexCoord");

    GLuint textureSamplerHandleY = (GLuint) glGetUniformLocation(programId, "yTexture");
    GLuint textureSamplerHandleU = (GLuint) glGetUniformLocation(programId, "uTexture");
    GLuint textureSamplerHandleV = (GLuint) glGetUniformLocation(programId, "vTexture");
    GLuint um4_mvp = glGetUniformLocation(programId,"um4_ModelViewProjection");


    //因为没有用矩阵所以就手动自适应
    int videoWidth = displayOpaque->displayParam->videoWidth;
    int videoHeight = displayOpaque->displayParam->videoHeight;

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
    glUniformMatrix4fv(um4_mvp, 1, GL_FALSE, displayOpaque->displayParam->matrix);

    /***
     * 开始解码
     * **/
    int ret;
    cloudVoiceLogD("display 2");
    while (displayOpaque->workFlag) {
        CloudVoiceAVPacket avPackect = (CloudVoiceAVPacket)displayOpaque->blockingQueue->take(displayOpaque->blockingQueue);
        if(avPackect && avPackect->data != NULL){
//            cloudVoiceLogD("display loop ------------ 2");
            yuvDataChange(display, avPackect);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, yTextureId);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, displayParam->videoWidth, displayParam->videoHeight,0, GL_LUMINANCE, GL_UNSIGNED_BYTE, displayOpaque->pixelsY);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, uTextureId);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,  displayParam->videoWidth/2, displayParam->videoHeight/2,0, GL_LUMINANCE, GL_UNSIGNED_BYTE, displayOpaque->pixelsU);

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, vTextureId);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,  displayParam->videoWidth/2, displayParam->videoHeight/2,0, GL_LUMINANCE, GL_UNSIGNED_BYTE, displayOpaque->pixelsV);


            /***
            * 纹理更新完成后开始绘制
            ***/
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            eglSwapBuffers(eglDisp, eglWindow);

        }
//        cloudVoiceLogD("display loop  destroy avpackect");
        cloudVoiceDestroyAVPackect(avPackect);
    }
    cloudVoiceLogD("display 3");
    eglMakeCurrent(eglDisp, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(eglDisp, eglCtx);
    eglDestroySurface(eglDisp, eglWindow);
    eglTerminate(eglDisp);
    eglDisp = EGL_NO_DISPLAY;
    eglWindow = EGL_NO_SURFACE;
    eglCtx = EGL_NO_CONTEXT;

    cloudVoiceLogI("display thread  finish");
}




static void setParam(CloudVoiceDisplay display,CloudVoiceDisplayParam displayParam){
    if(display && display->displayOpaque && displayParam){
        display->displayOpaque->displayParam = displayParam;
    }
}


static CloudVoiceDisplayParam getParam(CloudVoiceDisplay display){
    CloudVoiceDisplayParam displayParam = NULL;
    if(display && display->displayOpaque ){
        displayParam = display->displayOpaque->displayParam;
    }
    return displayParam;
}


static void setVideoSurface(CloudVoiceDisplay self,ANativeWindow* nativeWindow){
    if(nativeWindow){
        self->displayOpaque->aNativeWindow =nativeWindow;
    } else{

    }
}
static void setVideoMatrix(CloudVoiceDisplay self,float * matrixs){
    if(matrixs){
        self->displayOpaque->cacheMatrixTmp = matrixs;
    } else{

    }
}


static void setMediaStatusCallback(CloudVoiceDisplay display,MediaStatusCallback mediaStatusCallback){
    if(display && display->displayOpaque){
        display->displayOpaque->mediaStatusCallback = mediaStatusCallback;
    }
}

static void setMediaDataCallback(CloudVoiceDisplay display,MediaDataCallback mediaDataCallback){
    if(display && display->displayOpaque){
        display->displayOpaque->mediaDataCallback = mediaDataCallback;
    }
}

static void addData(CloudVoiceDisplay display,CloudVoiceAVPacket srcPackect){
    if(display && display ->displayOpaque && display->displayOpaque->workFlag == 1){
//        cloudVoiceLogD("display add data");
        display->displayOpaque->blockingQueue->add(display->displayOpaque->blockingQueue,srcPackect);
    } else{
        cloudVoiceDestroyAVPackect(srcPackect);
    }
}




static void start(CloudVoiceDisplay display){
    if (display && display->displayOpaque){
        display->displayOpaque->workFlag = 1;
        int retval = pthread_create(&display->displayOpaque->workThreadId,NULL,workThreadFunc,display);
//
        cloudVoiceLogI("start display thread retval : %d", retval);
    }
}

static void stop(CloudVoiceDisplay display){
    if (display && display->displayOpaque){
        if(display->displayOpaque->workThreadId != -1){
            cloudVoiceLogI("pthread_join  display start");
            display->displayOpaque->workFlag = 0;
            display->displayOpaque->blockingQueue->clear(display->displayOpaque->blockingQueue);
            pthread_join(display->displayOpaque->workThreadId,NULL);


            cloudVoiceLogI("pthread_join display finish");
            display->displayOpaque->workThreadId = -1;
        }
    }
}


static void destroy(CloudVoiceDisplay display){
    if (display && display->displayOpaque){
        cloudVoiceLogD("display destroy start");
        stop(display);
        if (display->displayOpaque->cacheMatrixTmp){
            free(display->displayOpaque->cacheMatrixTmp);
            display->displayOpaque->cacheMatrixTmp = NULL;
        }
        if(display->displayOpaque->aNativeWindow){
            ANativeWindow_release(display->displayOpaque->aNativeWindow);
            display->displayOpaque->aNativeWindow = NULL;
        }

        if (display->displayOpaque->displayParam){
            cloudVoiceDestroyDisplayParam(display->displayOpaque->displayParam);
            display->displayOpaque->displayParam = NULL;
        }

        if (display->displayOpaque->blockingQueue){
            display->displayOpaque->blockingQueue->destroy(display->displayOpaque->blockingQueue);
            display->displayOpaque->blockingQueue = NULL;
        }
        display->externHandle = NULL;
        free(display->displayOpaque);
        free(display);
        cloudVoiceLogD("display destroy finish");
    }
}

static void cloudVoiceAVPackactFreeCallback(void *object, void* avPackect){
    cloudVoiceDestroyAVPackect((CloudVoiceAVPacket)avPackect);
}

CloudVoiceDisplay cloudVoiceCreateAndroidVideoDisplay(/*CloudVoiceDisplayParam displayParam*/){
    CloudVoiceDisplay  display = NULL;
    display = (CloudVoiceDisplay)malloc(sizeof(CloudVoiceDisplay_));
    if(display){
        CloudVoiceDisplayOpaque displayOpaque = (CloudVoiceDisplayOpaque)malloc(sizeof(CloudVoiceDisplayOpaque_));
        if(display&&displayOpaque){
//            displayOpaque->displayParam = displayParam;
            displayOpaque->workFlag = 0;
            displayOpaque->displayParam = NULL;
            displayOpaque->blockingQueue = NULL;
            displayOpaque->workThreadId = -1;
            displayOpaque->aNativeWindow = NULL;
            displayOpaque->cacheMatrixTmp = NULL;
            displayOpaque->mediaDataCallback = NULL;
            displayOpaque->mediaStatusCallback = NULL;
            displayOpaque->pixelsY = NULL;
            displayOpaque->pixelsU = NULL;
            displayOpaque->pixelsV = NULL;
            displayOpaque->displayParam = cloudVoiceCreateDisplayParam();
            displayOpaque->blockingQueue = cloudVoiceCreateBlockingQueue(cloudVoiceAVPackactFreeCallback);

            display->getParam = getParam;
            display->setParam = setParam;
            display->setVideoSurface = setVideoSurface;
            cloudVoiceLogD("display : %p, setVideoSurface : %p",display,setVideoSurface);

            display->setVideoMatrix = setVideoMatrix;
            display->setMediaStatusCallback = setMediaStatusCallback;
            display->setMediaDataCallback = setMediaDataCallback;
            display->addData = addData;
            cloudVoiceLogD("display : %p, addData : %p",display,addData);
            display->start = start;
            display->stop = stop;
            display->destroy = destroy;
            display->externHandle = NULL;



            display->displayOpaque = displayOpaque;
        }
    }
    return display;
}