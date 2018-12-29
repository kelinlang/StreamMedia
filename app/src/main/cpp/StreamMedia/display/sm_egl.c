//
// Created by dengjun on 2018/11/16.
//

#include <malloc.h>
#include "sm_egl.h"
#include "../../CommonLib/log/cl_sdl_log.h"
#include <android/native_window.h>
#include <string.h>

static EGLBoolean SM_EGL_isValid(SmEGL egl)
{
    if (egl &&
        egl->window &&
        egl->display &&
        egl->surface &&
        egl->context) {
        return EGL_TRUE;
    }

    return EGL_FALSE;
}

static int IJK_EGL_getSurfaceWidth(SmEGL egl)
{
    EGLint width = 0;
    if (!eglQuerySurface(egl->display, egl->surface, EGL_WIDTH, &width)) {
        LOGE("[EGL] eglQuerySurface(EGL_WIDTH) returned error %d", eglGetError());
        return 0;
    }

    return width;
}

static int IJK_EGL_getSurfaceHeight(SmEGL egl)
{
    EGLint height = 0;
    if (!eglQuerySurface(egl->display, egl->surface, EGL_HEIGHT, &height)) {
        LOGE("[EGL] eglQuerySurface(EGL_HEIGHT) returned error %d", eglGetError());
        return 0;
    }

    return height;
}

static EGLBoolean IJK_EGL_setSurfaceSize(SmEGL egl, int width, int height)
{
    if (!SM_EGL_isValid(egl))
        return EGL_FALSE;

#ifdef __ANDROID__
    egl->width  = IJK_EGL_getSurfaceWidth(egl);
    egl->height = IJK_EGL_getSurfaceHeight(egl);

    if (width != egl->width || height != egl->height) {
        int format = ANativeWindow_getFormat(egl->window);
        LOGI("ANativeWindow_setBuffersGeometry(w=%d,h=%d) -> (w=%d,h=%d);",
              egl->width, egl->height,
              width, height);
        int ret = ANativeWindow_setBuffersGeometry(egl->window, width, height, format);
        if (ret) {
            LOGE("[EGL] ANativeWindow_setBuffersGeometry() returned error %d", ret);
            return EGL_FALSE;
        }

        egl->width  = IJK_EGL_getSurfaceWidth(egl);
        egl->height = IJK_EGL_getSurfaceHeight(egl);
        return (egl->width && egl->height) ? EGL_TRUE : EGL_FALSE;
    }

    return EGL_TRUE;
#else
    // FIXME: other platform?
#endif
    return EGL_FALSE;
}

/*static EGLBoolean IJK_EGL_makeCurrent(SmEGL egl, EGLNativeWindowType window)
{
    if (window && window == egl->window &&
        egl->display &&
        egl->surface &&
        egl->context) {

        if (!eglMakeCurrent(egl->display, egl->surface, egl->surface, egl->context)) {
            LOGE("[EGL] elgMakeCurrent() failed (cached)\n");
            return EGL_FALSE;
        }

        return EGL_TRUE;
    }

    sm_egl_terminate(egl);
    egl->window = window;

    if (!window)
        return EGL_FALSE;

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        LOGE("[EGL] eglGetDisplay failed\n");
        return EGL_FALSE;
    }


    EGLint major, minor;
    if (!eglInitialize(display, &major, &minor)) {
        LOGE("[EGL] eglInitialize failed\n");
        return EGL_FALSE;
    }
    LOGI("[EGL] eglInitialize %d.%d\n", (int)major, (int)minor);


    static const EGLint configAttribs[] = {
            EGL_RENDERABLE_TYPE,    EGL_OPENGL_ES2_BIT,
            EGL_SURFACE_TYPE,       EGL_WINDOW_BIT,
            EGL_BLUE_SIZE,          8,
            EGL_GREEN_SIZE,         8,
            EGL_RED_SIZE,           8,
            EGL_NONE
    };

    static const EGLint contextAttribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
    };

    EGLConfig config;
    EGLint numConfig;
    if (!eglChooseConfig(display, configAttribs, &config, 1, &numConfig)) {
        LOGE("[EGL] eglChooseConfig failed\n");
        eglTerminate(display);
        return EGL_FALSE;
    }

#ifdef __ANDROID__
    {
        EGLint native_visual_id = 0;
        if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &native_visual_id)) {
            LOGE("[EGL] eglGetConfigAttrib() returned error %d", eglGetError());
            eglTerminate(display);
            return EGL_FALSE;
        }

        int32_t width  = ANativeWindow_getWidth(window);
        int32_t height = ANativeWindow_getWidth(window);
        LOGI("[EGL] ANativeWindow_setBuffersGeometry(f=%d);", native_visual_id);
        int ret = ANativeWindow_setBuffersGeometry(window, width, height, native_visual_id);
        if (ret) {
            LOGE("[EGL] ANativeWindow_setBuffersGeometry(format) returned error %d", ret);
            eglTerminate(display);
            return EGL_FALSE;
        }
    }
#endif

    EGLSurface surface = eglCreateWindowSurface(display, config, window, NULL);
    if (surface == EGL_NO_SURFACE) {
        LOGE("[EGL] eglCreateWindowSurface failed\n");
        eglTerminate(display);
        return EGL_FALSE;
    }

    EGLSurface context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
    if (context == EGL_NO_CONTEXT) {
        LOGE("[EGL] eglCreateContext failed\n");
        eglDestroySurface(display, surface);
        eglTerminate(display);
        return EGL_FALSE;
    }

    if (!eglMakeCurrent(display, surface, surface, context)) {
        LOGE("[EGL] elgMakeCurrent() failed (new)\n");
        eglDestroyContext(display, context);
        eglDestroySurface(display, surface);
        eglTerminate(display);
        return EGL_FALSE;
    }

#if 0
    #if defined(__ANDROID__)
    {
        const char *extensions = (const char *) glGetString(GL_EXTENSIONS);
        if (extensions) {
            char *dup_extensions = strdup(extensions);
            if (dup_extensions) {
                char *brk = NULL;
                char *ext = strtok_r(dup_extensions, " ", &brk);
                while (ext) {
                    if (0 == strcmp(ext, "GL_EXT_texture_rg"))
                        egl->gles2_extensions[IJK_GLES2__GL_EXT_texture_rg] = 1;

                    ext = strtok_r(NULL, " ", &brk);
                }

                free(dup_extensions);
            }
        }
    }
#elif defined(__APPLE__)
    egl->gles2_extensions[IJK_GLES2__GL_EXT_texture_rg] = 1;
#endif

    ALOGI("[EGL] GLES2 extensions begin:\n");
    ALOGI("[EGL]     GL_EXT_texture_rg: %d\n", egl->gles2_extensions[IJK_GLES2__GL_EXT_texture_rg]);
    ALOGI("[EGL] GLES2 extensions end.\n");
#endif

    Sm_GLES2_Renderer_setupGLES();

    egl->context = context;
    egl->surface = surface;
    egl->display = display;
    return EGL_TRUE;
}*/

int SmEglSetVideoParam(SmEGL egl,SmVideoParam videoParam1){
    if (egl != NULL){
        egl->videoParam = videoParam1;
        return 0;
    } else{
        return -1;
    }
}

SmVideoParam SmEglGetVideoParam(SmEGL egl){
    if (egl != NULL){
        return egl->videoParam;
    } else{
        return NULL;
    }
}


static int EglInitInternal(SmEGL egl,EGLNativeWindowType window){
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        LOGE("[EGL] eglGetDisplay failed\n");
        return EGL_FALSE;
    }
    egl->display = display;
    LOGV("eglGetDisplay ok");

    EGLint major, minor;
    if (!eglInitialize(display, &major, &minor)) {
        LOGE("[EGL] eglInitialize failed\n");
        return EGL_FALSE;
    }
    LOGI("[EGL] eglInitialize %d.%d\n", (int)major, (int)minor);
    LOGV("eglInitialize ok");

    static const EGLint configAttribs[] = {
            EGL_RENDERABLE_TYPE,    EGL_OPENGL_ES2_BIT,
            EGL_SURFACE_TYPE,       EGL_WINDOW_BIT,
            EGL_BLUE_SIZE,          8,
            EGL_GREEN_SIZE,         8,
            EGL_RED_SIZE,           8,
            EGL_NONE
    };

    static const EGLint contextAttribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
    };

    EGLConfig config;
    EGLint numConfig;
    if (!eglChooseConfig(display, configAttribs, &config, 1, &numConfig)) {
        LOGE("[EGL] eglChooseConfig failed\n");
        return EGL_FALSE;
    }
    LOGV("eglChooseConfig ok");


    EGLSurface surface = eglCreateWindowSurface(display, config, window, NULL);
    if (surface == EGL_NO_SURFACE) {
        LOGE("[EGL] eglCreateWindowSurface failed\n");
        eglTerminate(display);
        return EGL_FALSE;
    }
    egl->surface = surface;
    LOGV("eglCreateWindowSurface ok");


    EGLSurface context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
    if (context == EGL_NO_CONTEXT) {
        LOGE("[EGL] eglCreateContext failed\n");
        eglDestroySurface(display, surface);
        eglTerminate(display);
        return EGL_FALSE;
    }
    egl->context = context;
    LOGV("eglCreateContext ok");

    if (!eglMakeCurrent(display, surface, surface, context)) {
        LOGE("[EGL] elgMakeCurrent() failed (new)\n");
//        eglDestroyContext(display, context);
//        eglDestroySurface(display, surface);
//        eglTerminate(display);
        return EGL_FALSE;
    }
    LOGV("eglMakeCurrent ok");

    Sm_GLES2_Renderer_setupGLES();

    return EGL_TRUE;
}

static  int SmEglInit(SmEGL egl){
    int retVal = -1;
    if(egl){
        egl->gles2Impl = SmGles2ImplCreate();
        if (egl->gles2Impl){
            egl->gles2Impl->SmGles2SetVideoParam(egl->gles2Impl,egl->videoParam);
            retVal = egl->gles2Impl->SmGles2Init(egl->gles2Impl);
            if (retVal == 0 && egl->window != NULL){
                retVal = EglInitInternal(egl,egl->window);
            }
        }
    }
    return retVal;
}


void SmEglRelease(SmEGL egl){
    if (!SM_EGL_isValid(egl))
        return;
    if (egl->gles2Impl){
        egl->gles2Impl->SmGles2Release(egl->gles2Impl);
    }

    if (egl->display){
        eglMakeCurrent(egl->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (egl->context)
            eglDestroyContext(egl->display, egl->context);
        if (egl->surface)
            eglDestroySurface(egl->display, egl->surface);
        eglTerminate(egl->display);
        eglReleaseThread(); // FIXME: call at thread exit
    }

    egl->context = EGL_NO_CONTEXT;
    egl->surface = EGL_NO_SURFACE;
    egl->display = EGL_NO_DISPLAY;
}

EGLBoolean  SmEglDisplay(SmEGL egl, SmVideoData videoData){
    if(egl != NULL){
        if (egl ->gles2Impl != NULL){
            if (egl->gles2Impl->SmGles2Display(egl->gles2Impl,videoData) == GL_TRUE ){
                eglSwapBuffers(egl->display, egl->surface);
                return EGL_TRUE;
            } else{
                LOGE("[EGL] SmGles2Display failed\n");
                return EGL_FALSE;
            }
        } else{
            return EGL_FALSE;
        }
    } else{
        return EGL_FALSE;
    }
}

SmEGL  smCreateEgl(){
    SmEGL gles2Impl = (SmEGL)malloc(sizeof(SmEGL_));
    if(gles2Impl != NULL){
        gles2Impl->EglSetVideoParam = SmEglSetVideoParam;
        gles2Impl->EglGetVideoParam = SmEglGetVideoParam;
        gles2Impl->EglInit = SmEglInit;
        gles2Impl->EglRelease = SmEglRelease;
        gles2Impl->EglDisplay = SmEglDisplay;
        return gles2Impl;
    } else{
        LOGE("SmGles2Impl malloc fail");
        return NULL;
    }
}









void sm_egl_terminate(SmEGL egl){
    if (!SM_EGL_isValid(egl))
        return;
    if (egl->gles2Impl){
        egl->gles2Impl->SmGles2Release(egl->gles2Impl);
    }

    if (egl->display){
        eglMakeCurrent(egl->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (egl->context)
            eglDestroyContext(egl->display, egl->context);
        if (egl->surface)
            eglDestroySurface(egl->display, egl->surface);
        eglTerminate(egl->display);
        eglReleaseThread(); // FIXME: call at thread exit
    }

    egl->context = EGL_NO_CONTEXT;
    egl->surface = EGL_NO_SURFACE;
    egl->display = EGL_NO_DISPLAY;
}