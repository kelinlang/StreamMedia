//
// Created by dengjun on 2019/3/26.
//

#include <malloc.h>
#include "cloudvoice_stream_param.h"


CloudVoiceStreamParam cloudVoiceCreateStreamParam(){
    CloudVoiceStreamParam  StreamParam = (CloudVoiceStreamParam)malloc(sizeof(CloudVoiceStreamParam_));
    return StreamParam;
}

void  cloudVoiceDestroyStreamParam(CloudVoiceStreamParam StreamParam){
    if (StreamParam){
        if (StreamParam->url){
            free(StreamParam->url);
        }
        free(StreamParam);
    }
}