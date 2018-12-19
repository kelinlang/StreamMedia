//
// Created by dengjun on 2018/12/8.
//

#include <GLES2/gl2.h>
#include "gles2_internal.h"
#include "../../../CommonLib/log/cl_sdl_log.h"
#include "../../model/sm_video_data.h"
#include "./color.c"
#include "../../model/sm_video_fourcc.h"


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

static  GLboolean yunv420pPrepare(SmGles2Impl gles2Impl){
    if (!gles2Impl){
        return GL_FALSE;
    }
    LOGI("yunv420pPrepare");

    glPixelStorei(GL_UNPACK_ALIGNMENT,1);

    glUseProgram(gles2Impl->program);
    SmGles2CheckErrorTrace("glUseProgram");

    if (0 == gles2Impl->plane_textures[0]){
        glGenTextures(3,gles2Impl->plane_textures);
    }

    for(int i = 0; i < 3; ++i){
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, gles2Impl->plane_textures[i]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glUniform1i(gles2Impl->us2_sampler[i], i);
    }

    glUniformMatrix3fv(gles2Impl->um3_color_conversion, 1, GL_FALSE, SM_GLES2_getColorMatrix_bt709());

    return GL_TRUE;
}


static GLboolean yunv420pDisplay(SmGles2Impl gles2Impl,SmVideoData videoData){
    if (!gles2Impl || ! videoData || !videoData->pixels){
        return GL_FALSE;
    }
    int planes[3] = {0,1,2};
    const GLsizei widths[3]    = { videoData->pitches[0], videoData->pitches[1], videoData->pitches[2] };
    const GLsizei heights[3]   = { videoData->height,          videoData->height / 2,      videoData->height / 2 };
    const GLubyte *pixels[3]   = { videoData->pixels[0],  videoData->pixels[1],  videoData->pixels[2] };

    switch (videoData->dataFormat) {
        case SM_VIDEO_FCC_I420:
            break;
        case SM_VIDEO_FCC_YV12:
            planes[1] = 2;
            planes[2] = 1;
            break;
        default:
            LOGE("[yuv420p] unexpected format %x\n", videoData->dataFormat);
            return GL_FALSE;
    }

    for (int i = 0; i < 3; ++i) {
        int plane = planes[i];

        glBindTexture(GL_TEXTURE_2D, gles2Impl->plane_textures[i]);

        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_LUMINANCE,
                     widths[plane],
                     heights[plane],
                     0,
                     GL_LUMINANCE,
                     GL_UNSIGNED_BYTE,
                     pixels[plane]);
    }

    return GL_TRUE;
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


        //可以用switch case支持多重格式，目前yuv420p
        gles2Impl->us2_sampler[0] = glGetUniformLocation(gles2Impl->program, "us2_SamplerX");
        SmGles2CheckErrorTrace("glGetUniformLocation(us2_SamplerX)");
        gles2Impl->us2_sampler[1] = glGetUniformLocation(gles2Impl->program, "us2_SamplerY");
        SmGles2CheckErrorTrace("glGetUniformLocation(us2_SamplerY)");
        gles2Impl->us2_sampler[2] = glGetUniformLocation(gles2Impl->program, "us2_SamplerZ");
        SmGles2CheckErrorTrace("glGetUniformLocation(us2_SamplerZ)");

        gles2Impl->um3_color_conversion = glGetUniformLocation(gles2Impl->program, "um3_ColorConversion");
        SmGles2CheckErrorTrace("glGetUniformLocation(um3_ColorConversionMatrix)");
        //目前yuv420p


        return 0;
    } else{
        LOGE("gles2Impl is NULL");
        return -1;
    }
}

static void SmGles2Release(SmGles2Impl gles2Impl){
    if(gles2Impl != NULL){
        free(gles2Impl);
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