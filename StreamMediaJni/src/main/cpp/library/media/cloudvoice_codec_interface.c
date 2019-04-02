//
// Created by dengjun on 2019/3/29.
//

#include <malloc.h>
#include <memory.h>
#include "cloudvoice_codec_interface.h"

CloudVoiceCodecParam cloudVoiceCreateCodecParam(){
    CloudVoiceCodecParam codecParam = (CloudVoiceCodecParam)malloc(sizeof(CloudVoiceCodecParam_));
    if (codecParam){
        memset(codecParam,0, sizeof(CloudVoiceCodecParam_));
    }
    return codecParam;
}

void cloudVoiceDestroyCodecParam(CloudVoiceCodecParam codecParam){
    if(codecParam){
        free(codecParam);
    }
}
