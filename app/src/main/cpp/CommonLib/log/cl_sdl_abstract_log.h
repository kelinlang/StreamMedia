//
// Created by dengjun on 2018/12/1.
//

#ifndef STREAMMEDIADEMO_CL_SDL_ABSTRACT_LOG_H
#define STREAMMEDIADEMO_CL_SDL_ABSTRACT_LOG_H

#include <stdio.h>

typedef void(*AbstractLogFuncI)(char *, int);
static AbstractLogFuncI  log_func_i;


void SetAbstractLogI(AbstractLogFuncI logfunci);

void AbstractLogI(char *logString, int logValue);



#endif //STREAMMEDIADEMO_CL_SDL_ABSTRACT_LOG_H
