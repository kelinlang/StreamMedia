//
// Created by dengjun on 2018/11/16.
//

#include "sm_gles2_display.h"
#include "gles2/gles2_internal.h"
#include "../../CommonLib/log/cl_sdl_abstract_log.h"


static void SmGles2RendererReset(SmGles2Renderer gles2Renderer){

}
static void SmGles2RendererFree(SmGles2Renderer gles2Renderer){

}
static void SmGles2RendererFreep(SmGles2Renderer gles2Renderer){

}

static GLboolean SmGles2RendererSetupGles2(SmGles2Renderer gles2Renderer){
    return GL_TRUE;
}

static GLboolean SmGles2RendererIsValid(SmGles2Renderer gles2Renderer){
    return GL_TRUE;
}

static GLboolean SmGles2RendererSetParam(SmGles2Renderer gles2Renderer, SmGles2Param *gles2Param){
    return GL_TRUE;
}

static SmGles2Param *SmGles2RendererGetParam(SmGles2Renderer gles2Renderer){
    return NULL;
}

static GLboolean SmGles2RendererUse(SmGles2Renderer gles2Renderer){
    return GL_TRUE;
}

static GLboolean SmGles2RendererRender(SmGles2Renderer gles2Renderer, SmVideoData *videoData){
    return GL_TRUE;
}


SmGles2Renderer  SmGles2RendererCreate(SmGles2Param * gles2Param){
    if (!gles2Param){
        return NULL;
    }
    SmGles2PrintString("Version", GL_VERSION);
    SmGles2PrintString("Vendor", GL_VENDOR);
    SmGles2PrintString("Renderer", GL_RENDERER);
    SmGles2PrintString("Extensions", GL_EXTENSIONS);

    SmGles2Renderer gles2Renderer = malloc(sizeof(SmGles2Renderer_));
    if(!gles2Renderer){
        gles2Renderer->gles2Param = gles2Param;
        gles2Renderer->SmGles2RendererReset = SmGles2RendererReset;
        gles2Renderer->SmGles2RendererFree = SmGles2RendererFree;
        gles2Renderer->SmGles2RendererFreep = SmGles2RendererFreep;
        gles2Renderer->SmGles2RendererSetupGles2 = SmGles2RendererSetupGles2;
        gles2Renderer->SmGles2RendererIsValid = SmGles2RendererIsValid;
        gles2Renderer->SmGles2RendererSetParam = SmGles2RendererSetParam;
        gles2Renderer->SmGles2RendererGetParam = SmGles2RendererGetParam;
        gles2Renderer->SmGles2RendererUse = SmGles2RendererUse;
        gles2Renderer->SmGles2RendererRender = SmGles2RendererRender;
    } else{
        AbstractLogI("SmGles2RendererCreate malloc fail",0);
    }
    return NULL;
}