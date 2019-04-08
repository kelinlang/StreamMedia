//
// Created by dengjun on 2019/3/28.
//

#include <memory.h>
#include <malloc.h>
#include "cloudvoice_utils.h"

void cloudVoiceStringCopy(const char* src, char ** dst){
    if(src && dst){
        int len = strlen(src)+1;
        * dst = (char*)malloc(len);
        strcpy(*dst,src);
    }
}