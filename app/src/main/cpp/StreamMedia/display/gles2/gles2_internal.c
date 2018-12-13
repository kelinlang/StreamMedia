//
// Created by dengjun on 2018/12/8.
//

#include <GLES2/gl2.h>
#include "gles2_internal.h"
#include "../../../CommonLib/log/cl_sdl_log.h"
#include "../../model/sm_video_data.h"




static int SmGles2SetVideoParam(SmGles2Impl gles2Impl,SmVideoParam videoParam){
    if (gles2Impl != NULL){
        gles2Impl->videoParam = videoParam;
        return 0;
    } else{
        return -1;
    }
}

static SmVideoParam SmGles2GetVideoParam(SmGles2Impl gles2Impl){
    if (gles2Impl != NULL){
        return gles2Impl->videoParam;
    } else{
        return NULL;
    }
}




static int SmGles2Init(SmGles2Impl gles2Impl){
    if(gles2Impl != NULL){
        gles2Impl->vertex_shader = SmGles2LoadShader(GL_VERTEX_SHADER,Sm_GLES2_getVertexShader_default());
        if(!gles2Impl->vertex_shader){
            LOGE("load GL_VERTEX_SHADER error");
            return -1;
        }

        gles2Impl->fragment_shader = SmGles2LoadShader(GL_FRAGMENT_SHADER,Sm_GLES2_getFragmentShader_yuv420p());
        if(!gles2Impl->fragment_shader){
            LOGE("load GL_FRAGMENT_SHADER error");
            return -1;
        }

        gles2Impl->program = glCreateProgram();
        SmGles2CheckError("glCreateProgram");
        if(!gles2Impl->program){
            LOGE("glCreateProgram error");
            return -1;
        }

        glAttachShader(gles2Impl->program, gles2Impl->vertex_shader);
        SmGles2CheckError("glAttachShader(vertex)");

        glAttachShader(gles2Impl->program,gles2Impl-> fragment_shader);
        SmGles2CheckError("glAttachShader(fragment)");

        glLinkProgram(gles2Impl-> program);

        GLint  link_status = GL_FALSE;
        glGetProgramiv(gles2Impl->program,GL_LINK_STATUS,&link_status);
        if(!link_status){
            LOGE("glGetProgramiv error");
            return -1;
        }

        gles2Impl->av4_position = glGetAttribLocation(gles2Impl->program,"av4_Position");
        SmGles2CheckErrorTrace("glGetAttribLocation(av4_Position)");

        gles2Impl->av2_texcoord = glGetAttribLocation(gles2Impl->program,"av2_Texcoord");
        SmGles2CheckErrorTrace("glGetAttribLocation(av2_Texcoord)");

        gles2Impl->um4_mvp = glGetUniformLocation(gles2Impl->program,"um4_ModelViewProjection");
        SmGles2CheckErrorTrace("glGetAttribLocation(um4_ModelViewProjection)");


        return 0;
    } else{
        LOGE("gles2Impl is NULL");
        return -1;
    }
}


int  SmGles2ImplCreate(SmGles2Impl gles2Impl){
    gles2Impl = (SmGles2Impl)malloc(sizeof(SmGles2Impl_));
    if(gles2Impl != NULL){
        gles2Impl->SmGles2SetVideoParam = SmGles2SetVideoParam;
        gles2Impl->SmGles2GetVideoParam = SmGles2GetVideoParam;
        gles2Impl->SmGles2Init = SmGles2Init;
        return 0;
    } else{
        LOGE("SmGles2Impl malloc fail");
        return -1;
    }
}