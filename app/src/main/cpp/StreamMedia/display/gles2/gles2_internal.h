//
// Created by dengjun on 2018/12/8.
//

#ifndef STREAMMEDIADEMO_GLES2_INTERNAL_H
#define STREAMMEDIADEMO_GLES2_INTERNAL_H


#include "../sm_gles2_display.h"

typedef struct SmGles2Impl_
{

    GLuint program;

    GLuint vertex_shader;
    GLuint fragment_shader;
    GLuint plane_textures[IJK_GLES2_MAX_PLANE];

    GLuint av4_position;
    GLuint av2_texcoord;
    GLuint um4_mvp;

    GLuint us2_sampler[IJK_GLES2_MAX_PLANE];
    GLuint um3_color_conversion;

    GLboolean (*func_use)(SmGles2Impl gles2Opaque);
    GLsizei   (*func_getBufferWidth)(SmGles2Impl gles2Opaque, SmVideoData *videoData);
    GLboolean (*func_uploadTexture)(SmGles2Impl gles2Opaque, SmVideoData *videoData);
    GLvoid    (*func_destroy)(SmGles2Impl gles2Opaque);

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
} SmGles2Opaque_;


typedef struct SmGlesMatrix
{
    GLfloat m[16];
} SmGlesMatrix;

void SmGles2PrintString(const char *name, GLenum s);
void SmGles2CheckError(const char* op);

void SmGLES2LoadOrtho(SmGlesMatrix *matrix, GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near, GLfloat far);

SmGles2Impl SmGles2ImplCreate(uint32_t format);



const char *Sm_GLES2_getVertexShader_default();
const char *Sm_GLES2_getFragmentShader_yuv420p();
const char *Sm_GLES2_getFragmentShader_yuv444p10le();
const char *Sm_GLES2_getFragmentShader_yuv420sp();
const char *Sm_GLES2_getFragmentShader_rgb();

const GLfloat *Sm_GLES2_getColorMatrix_bt709();
const GLfloat *Sm_GLES2_getColorMatrix_bt601();

SmGles2Impl SmGles2Opaque__create_base(const char *fragment_shader_source);
SmGles2Impl SmGles2Opaque__create_yuv420p();
SmGles2Impl SmGles2Opaque__create_yuv444p10le();
SmGles2Impl SmGles2Opaque__create_yuv420sp();
SmGles2Impl SmGles2Opaque__create_yuv420sp_vtb(SmVideoData *videoData);
SmGles2Impl SmGles2Opaque__create_rgb565();
SmGles2Impl SmGles2Opaque__create_rgb888();
SmGles2Impl SmGles2Opaque__create_rgbx8888();



#endif //STREAMMEDIADEMO_INTERNAL_H
