//
// Created by dengjun on 2018/11/23.
//
#include "gles2_internal.h"
#include "../../../CommonLib/log/cl_sdl_abstract_log.h"


void SmGles2CheckError( char* op){
    for (GLint error = glGetError(); error; error = glGetError()) {
        AbstractLogI(op, error);
    }
}

void SmGles2PrintString( char *name, GLenum s) {
     char *v = ( char *) glGetString(s);
    AbstractLogI(name, 0);
    AbstractLogI(v, 0);
}

void SmGLES2LoadOrtho(SmGlesMatrix *matrix, GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near, GLfloat far)
{
    GLfloat r_l = right - left;
    GLfloat t_b = top - bottom;
    GLfloat f_n = far - near;
    GLfloat tx = - (right + left) / (right - left);
    GLfloat ty = - (top + bottom) / (top - bottom);
    GLfloat tz = - (far + near) / (far - near);

    matrix->m[0] = 2.0f / r_l;
    matrix->m[1] = 0.0f;
    matrix->m[2] = 0.0f;
    matrix->m[3] = 0.0f;

    matrix->m[4] = 0.0f;
    matrix->m[5] = 2.0f / t_b;
    matrix->m[6] = 0.0f;
    matrix->m[7] = 0.0f;

    matrix->m[8] = 0.0f;
    matrix->m[9] = 0.0f;
    matrix->m[10] = -2.0f / f_n;
    matrix->m[11] = 0.0f;

    matrix->m[12] = tx;
    matrix->m[13] = ty;
    matrix->m[14] = tz;
    matrix->m[15] = 1.0f;
}
