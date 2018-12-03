//
// Created by dengjun on 2018/12/1.
//

#include "cl_sdl_abstract_log.h"


static abstract_log_func_I  log_func_i;

void set_abstract_log_i(abstract_log_func_I  logfunci){
    if (log_func_i == NULL){
        log_func_i = logfunci;
    }
}

void abstract_log_i(char * logString, int  logValue){
    if(log_func_i != NULL){
        log_func_i(logString, logValue);
    }
};