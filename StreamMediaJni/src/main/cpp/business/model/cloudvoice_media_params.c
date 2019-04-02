//
// Created by dengjun on 2019/3/26.
//

#include <malloc.h>
#include "cloudvoice_media_params.h"

CloudVoiceMediaParam cloudVoiceCreateMediaParam(){
    CloudVoiceMediaParam  MediaParam = (CloudVoiceMediaParam)malloc(sizeof(CloudVoiceMediaParam_));
    return MediaParam;
}

void  cloudVoiceDestroyMediaParam(CloudVoiceMediaParam MediaParam){
    if (MediaParam){
        if (MediaParam->url){
            free(MediaParam->url);
        }
        free(MediaParam);
    }
}


