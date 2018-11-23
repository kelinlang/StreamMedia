//
// Created by dengjun on 2018/11/16.
//

#ifndef STREAMMEDIADEMO_SM_EGL_H
#define STREAMMEDIADEMO_SM_EGL_H

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>
#include "../../CommonLib/common/cl_common_class.h"


typedef struct SmEGLOpaque SmEGLOpaque;

typedef struct SmEGL{
    CommonClass *opaque_class;

    SmEGLOpaque *opaque;

    EGLNativeWindowType  window_type;

    EGLDisplay  display;
    EGLSurface  surface;
    EGLContext context;

    EGLint width;
    EGLint height;
}SmEGL;


SmEGL *sm_egl_create();
void sm_egl_free(SmEGL *egl);
void sm_egl_freep(SmEGL **egl);
EGLBoolean  sm_egl_display(SmEGL *egl,EGLNativeWindowType windowType);
void sm_egl_terminate(SmEGL *egl);

#endif //STREAMMEDIADEMO_SM_EGL_H
