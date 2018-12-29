//
// Created by dengjun on 2018/11/16.
//

#include <malloc.h>
#include "sm_video_data.h"


SmVideoData smCreateVideoData(){
    SmVideoData videoData = (SmVideoData)malloc(sizeof(SmVideoData_));
    if(videoData){
        videoData->pixels = NULL;
        videoData->pitches = NULL;
    }
    return videoData;
}
void smDestroyVideoData(SmVideoData videoData){
    if (videoData){
        if(videoData->pixels){
            free(videoData->pixels);
        }
        if(videoData->pitches){
            free(videoData->pitches);
        }
    }
}


SmVideoParam smCreateVideoParam(){
    SmVideoParam videoParam = (SmVideoParam)malloc(sizeof(SmVideoParam_));
    if(videoParam){

    }
    return videoParam;
}
void smDestroyVideoParam(SmVideoParam videoParam){
    if(videoParam){
        free(videoParam);
    }
}