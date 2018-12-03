//
// Created by dengjun on 2018/12/1.
//

#ifndef STREAMMEDIADEMO_CL_SDL_ABSTRACT_LOG_H
#define STREAMMEDIADEMO_CL_SDL_ABSTRACT_LOG_H

#include <stdio.h>

typedef void(*abstract_log_func_I)(char *, int);
static abstract_log_func_I  log_func_i;


void set_abstract_log_i(abstract_log_func_I  logfunci);

void abstract_log_i(char * logString, int  logValue);



#endif //STREAMMEDIADEMO_CL_SDL_ABSTRACT_LOG_H
