//
// Created by dengjun on 2019/3/27.
//

#ifndef VOICELIBDEMO_CLOUDVOICE_SHADER_UTILS_H
#define VOICELIBDEMO_CLOUDVOICE_SHADER_UTILS_H


#include <GLES2/gl2.h>

GLuint cloudVoiceCreateProgram(const char *vertexSource, const char *fragmentSource);

GLuint cloudVoidceLoadShader(GLenum shaderType, const char *source);
#endif //VOICELIBDEMO_CLOUDVOICE_SHADER_UTILS_H
