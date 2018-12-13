//
// Created by dengjun on 2018/11/16.
//

#ifndef STREAMMEDIADEMO_SM_GLES2_DISPLAY_H
#define STREAMMEDIADEMO_SM_GLES2_DISPLAY_H

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES2/gl2platform.h>
#include <malloc.h>

#include "../CommonLib/log/cl_sdl_abstract_log.h"
#include "../model/sm_video_data.h"
#include "gles2/gles2_internal.h"


/*
 * Renderer
 */


#define SM_GLES2_GRAVITY_RESIZE                (0) // Stretch to fill view bounds.
#define SM_GLES2_GRAVITY_RESIZE_ASPECT         (1) // Preserve aspect ratio; fit within view bounds.
#define SM_GLES2_GRAVITY_RESIZE_ASPECT_FILL    (2) // Preserve aspect ratio; fill view bounds.

typedef struct SmGles2Param{
    GLsizei viewWidth;
    GLsizei viewHeight;
    int gravity;

    uint32_t format;
}SmGles2Param;

typedef struct SmGles2Renderer_ *SmGles2Renderer;
//具体实现相关

typedef struct SmGles2Renderer_{
    SmGles2Param *gles2Param;
     SmGles2Impl gles2Opaque;

    void (*SmGles2RendererReset)(SmGles2Renderer gles2Renderer);
    void (*SmGles2RendererFree)(SmGles2Renderer gles2Renderer);
    void (*SmGles2RendererFreep)(SmGles2Renderer gles2Renderer);

    GLboolean (*SmGles2RendererSetupGles2)(SmGles2Renderer gles2Renderer);
    GLboolean (*SmGles2RendererIsValid)(SmGles2Renderer gles2Renderer);
    GLboolean (*SmGles2RendererSetParam)(SmGles2Renderer gles2Renderer, SmGles2Param *gles2Param);
    SmGles2Param *(*SmGles2RendererGetParam)(SmGles2Renderer gles2Renderer);

    GLboolean (*SmGles2RendererUse)(SmGles2Renderer gles2Renderer);

    GLboolean (*SmGles2RendererRender)(SmGles2Renderer gles2Renderer, SmVideoData *videoData);

}SmGles2Renderer_;




SmGles2Param* SmGles2Create();
void SmGles2Free(SmGles2Param * gles2Param);


SmGles2Renderer  SmGles2RendererCreate(SmGles2Param * gles2Param);
/*void SmGles2RendererReset(SmGles2Renderer *gles2Renderer);
void SmGles2RendererFree(SmGles2Renderer *gles2Renderer);
void SmGles2RendererFreep(SmGles2Renderer *gles2Renderer);

GLboolean SmGles2RendererSetupGles2();
GLboolean SmGles2RendererIsValid(SmGles2Renderer *gles2Renderer);
GLboolean SmGles2RendererSetParam(SmGles2Renderer *gles2Renderer, SmGles2Param gles2Param);
SmGles2Param *SmGles2RendererGetParam();

GLboolean SmGles2RendererUse(SmGles2Renderer *gles2Renderer);

GLboolean SmGles2RendererRender(SmGles2Renderer *gles2Renderer, SmVideoData *videoData);*/


#endif //STREAMMEDIADEMO_SM_GLES2_DISPLAY_H
