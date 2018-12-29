//
// Created by dengjun on 2018/12/8.
//

#ifndef STREAMMEDIADEMO_GLES2_INTERNAL_H
#define STREAMMEDIADEMO_GLES2_INTERNAL_H


#include <GLES2/gl2.h>

#include "../../model/sm_video_data.h"

#define SM_GLES_STRINGIZE(x)   #x
#define SM_GLES_STRINGIZE2(x)  SM_GLES_STRINGIZE(x)
#define SM_GLES_STRING(x)      SM_GLES_STRINGIZE2(x)

typedef struct SmGles2Impl_ * SmGles2Impl;

typedef struct SmGles2Impl_
{

    GLuint program;

    GLuint vertex_shader;
    GLuint fragment_shader;
    GLuint plane_textures[SM_GLES2_MAX_PLANE];

    GLuint av4_position;
    GLuint av2_texcoord;
    GLuint um4_mvp;

    GLuint us2_sampler[SM_GLES2_MAX_PLANE];
    GLuint um3_color_conversion;

    GLsizei buffer_width;
    GLsizei visible_width;

    GLfloat texcoords[8];

    GLfloat vertices[8];
    int     vertices_changed;

    int     format;
    int     gravity;
    GLsizei layer_width;
    GLsizei layer_height;
    int     frame_width;
    int     frame_height;
    int     frame_sar_num;
    int     frame_sar_den;

    GLsizei last_buffer_width;

    SmVideoParam videoParam;

    int (*SmGles2SetVideoParam)(SmGles2Impl gles2Impl,SmVideoParam videoParam1);
    SmVideoParam (*SmGles2GetVideoParam)(SmGles2Impl gles2Impl);
    int (*SmGles2Init)(SmGles2Impl gles2Impl);
    void (*SmGles2Release)(SmGles2Impl gles2Impl);


    GLboolean    (*SmGles2Display)(SmGles2Impl gles2Impl, SmVideoData videoData);
} SmGles2Impl_;


typedef struct SmGlesMatrix
{
    GLfloat m[16];
} SmGlesMatrix;


#ifdef DEBUG
#define SmGles2CheckErrorTrace(op)
#define SmGles2CheckErrorDebug(op)
#else
#define SmGles2CheckErrorTrace(op) SmGles2CheckError(op)
#define SmGles2CheckErrorDebug(op) SmGles2CheckError(op)
#endif

void SmGles2PrintString( char *name, GLenum s);
void SmGles2CheckError( char* op);

GLuint  SmGles2LoadShader(GLenum shader_type, const char *shader_source);


void SmGLES2LoadOrtho(SmGlesMatrix *matrix, GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near, GLfloat far);

SmGles2Impl  SmGles2ImplCreate( );

GLboolean Sm_GLES2_Renderer_setupGLES();
void      IJK_GLES2_Renderer_reset(SmGles2Impl renderer);
GLboolean IJK_GLES2_Renderer_isValid(SmGles2Impl renderer);
GLboolean IJK_GLES2_Renderer_isFormat(SmGles2Impl renderer, int format);



const char *Sm_GLES2_getVertexShader_default();
const char *Sm_GLES2_getFragmentShader_yuv420p();
const char *IJK_GLES2_getFragmentShader_rgb();
const char *IJK_GLES2_getFragmentShader_yuv420p();
const char *Sm_GLES2_getFragmentShader_yuv420p();
const char *IJK_GLES2_getFragmentShader_yuv444p10le();

const GLfloat *SM_GLES2_getColorMatrix_bt709();
const GLfloat *IJK_GLES2_getColorMatrix_bt601();

#endif //STREAMMEDIADEMO_INTERNAL_H
