//
// Created by dengjun on 2018/12/1.
//

#include "cl_sdl_abstract_log.h"


static AbstractLogFuncI  log_func_i;

void SetAbstractLogI(AbstractLogFuncI logfunci){
    if (log_func_i == NULL){
        log_func_i = logfunci;
    }
}

void AbstractLogI(char *logString, int logValue){
    if(log_func_i != NULL){
        log_func_i(logString, logValue);
    }
};