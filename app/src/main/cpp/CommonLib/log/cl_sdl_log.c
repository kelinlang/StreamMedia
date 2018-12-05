//
// Created by dengjun on 2018/11/15.
//

#include "cl_sdl_log.h"

void setDebug(jboolean isDebug) {
    debug_flag = isDebug;
}

static void cl_log_i(char * logString, int  logValue){
    if (logString != NULL){
        LOGI("logString: %s, logValue: %d\n", logString, logValue);
    } else{
        LOGI("logString: NULL, logValue: %d\n", logValue);
    }
}


void cl_init_log(){
    SetAbstractLogI(cl_log_i);
}