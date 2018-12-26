//
// Created by dengjun on 2018/11/16.
//

#ifndef STREAMMEDIADEMO_SM_EGL_H
#define STREAMMEDIADEMO_SM_EGL_H

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>
#include "../../CommonLib/common/cl_common_class.h"
#include "gles2/gles2_internal.h"
#include "../model/sm_video_data.h"


typedef struct SmEGLOpaque SmEGLOpaque;

typedef struct SmEGL_ * SmEGL;

typedef struct SmEGL_{
    CommonClass *opaque_class;

    SmGles2Impl gles2Impl;

    EGLNativeWindowType  window;

    EGLDisplay  display;
    EGLSurface  surface;
    EGLContext context;

    EGLint width;
    EGLint height;

    SmVideoParam videoParam;

    int (*EglSetVideoParam)(SmEGL egl,SmVideoParam videoParam1);
    SmVideoParam (*EglGetVideoParam)(SmEGL egl);
    int (*EglInit)(SmEGL egl);
    void (*EglRelease)(SmEGL egl);
    EGLBoolean    (*EglDisplay)(SmEGL gles2Impl, SmVideoData videoData);
}SmEGL_;


SmEGL  smCreateEgl();

/*SmEGL sm_egl_create();
void sm_egl_free(SmEGL egl);
void sm_egl_freep(SmEGL *egl);
EGLBoolean  sm_egl_display(SmEGL egl,EGLNativeWindowType windowType, SmVideoData SmVideoData);
void sm_egl_terminate(SmEGL egl);*/

#endif //STREAMMEDIADEMO_SM_EGL_H
