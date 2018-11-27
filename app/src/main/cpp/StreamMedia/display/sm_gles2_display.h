//
// Created by dengjun on 2018/11/16.
//

#ifndef STREAMMEDIADEMO_SM_GLES2_DISPLAY_H
#define STREAMMEDIADEMO_SM_GLES2_DISPLAY_H

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES2/gl2platform.h>
#include "../model/sm_video_data.h"

typedef struct SmGles2Renderer SmGles2Renderer;

#define SM_GLES2_GRAVITY_RESIZE                (0) // Stretch to fill view bounds.
#define SM_GLES2_GRAVITY_RESIZE_ASPECT         (1) // Preserve aspect ratio; fit within view bounds.
#define SM_GLES2_GRAVITY_RESIZE_ASPECT_FILL    (2) // Preserve aspect ratio; fill view bounds.

typedef struct SmGles2Param{
    GLsizei view_width;
    GLsizei view_height;
    int gravity;
}SmGles2Param;


SmGles2Param* sm_gles2_create();
void sm_gles2_free(SmGles2Param * gles2Param);


SmGles2Renderer * sm_gles2_renderer_create();
void sm_gles2_renderer_reset(SmGles2Renderer *gles2Renderer);
void sm_gles2_renderer_free(SmGles2Renderer *gles2Renderer);
void sm_gles2_renderer_freep(SmGles2Renderer *gles2Renderer);

GLboolean sm_gles2_renderer_setup_gles();
GLboolean sm_gles2_renderer_isValid(SmGles2Renderer *gles2Renderer);
GLboolean sm_gles2_renderer_set_param(SmGles2Renderer *gles2Renderer, SmGles2Param gles2Param);
SmGles2Param *sm_gles2_renderer_get_param();

GLboolean sm_gles2_renderer_use(SmGles2Renderer *gles2Renderer);

GLboolean sm_gles2_renderer_render(SmGles2Renderer *gles2Renderer, VideoData *videoData);


#endif //STREAMMEDIADEMO_SM_GLES2_DISPLAY_H
