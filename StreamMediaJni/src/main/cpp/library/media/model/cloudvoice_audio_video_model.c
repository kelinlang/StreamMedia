//
// Created by dengjun on 2019/3/21.
//

#include <malloc.h>
#include "cloudvoice_audio_video_model.h"


CloudVoiceAVPacket cloudVoiceCreateAVPackect(){
    CloudVoiceAVPacket avPackect = (CloudVoiceAVPacket)malloc(sizeof(CloudVoiceAVPacket_));
    if(avPackect){
        avPackect->presentTimeStamp = 0;
        avPackect->decodeTimeStamp = 0;
//        videoData->pitches = NULL;
        avPackect->dataLen = 0;
        avPackect->data = NULL;
        avPackect->startPos = 0;
//        avPackect->idLen = 0;
    }
    return avPackect;
}
void cloudVoiceDestroyAVPackect(CloudVoiceAVPacket avPackect){
    if(avPackect){
        if (avPackect->data){
            free(avPackect->data);
            avPackect->data = NULL;
        }

        if (avPackect->sps){
            free(avPackect->sps);
            avPackect->sps = NULL;
        }

        if (avPackect->pps){
            free(avPackect->pps);
            avPackect->pps = NULL;
        }
        free(avPackect);
    }
}

CloudVoiceVideoParam cloudVoiceCreateVideoParam(){
    CloudVoiceVideoParam videoParam = (CloudVoiceVideoParam)malloc(sizeof(CloudVoiceVideoParam_));
    if(videoParam){
        videoParam->viewWidth = 0;
        videoParam->viewHeight = 0;
        videoParam->videoWidth = 0;
        videoParam->videoHeight = 0;
        videoParam->dataFormat = 0;
        videoParam->displayFormat = 0;
        videoParam->matrixLen = 0;
        videoParam->matrix = NULL;
    }
    return videoParam;
}
void cloudVoiceDestroyVideoParam(CloudVoiceVideoParam videoParam){
    if(videoParam){
        if(videoParam->matrix){
            free(videoParam->matrix);
        }
        free(videoParam);
    }
}


CloudVoiceDisplayParam cloudVoiceCreateDisplayParam(){
    CloudVoiceDisplayParam displayParam = (CloudVoiceDisplayParam)malloc(sizeof(CloudVoiceDisplayParam_));
    if(displayParam){
        displayParam->viewWidth = 0;
        displayParam->viewHeight = 0;
        displayParam->videoWidth = 0;
        displayParam->videoHeight = 0;
        displayParam->dataFormat = 0;
        displayParam->displayFormat = 0;
        displayParam->matrixLen = 0;
        displayParam->matrix = NULL;
    }
    return displayParam;
}

void cloudVoiceDestroyDisplayParam(CloudVoiceDisplayParam displayParam){
    if(displayParam){
        if(displayParam->matrix){
            free(displayParam->matrix);
        }
        free(displayParam);
    }
}
