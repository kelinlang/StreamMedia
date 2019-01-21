//
// Created by dengjun on 2018/11/16.
//

#include <malloc.h>
#include "sm_video_data.h"


SmVideoData smCreateVideoData(){
    SmVideoData videoData = (SmVideoData)malloc(sizeof(SmVideoData_));
    if(videoData){
        videoData->pixels = NULL;
//        videoData->pitches = NULL;
        videoData->pixelsY = NULL;
        videoData->pixelsU = NULL;
        videoData->pixelsV = NULL;
        videoData->pixelsData = NULL;
    }
    return videoData;
}
void smDestroyVideoData(SmVideoData videoData){
    if (videoData){
        if(videoData->pixels){
            free(videoData->pixels);
            videoData->pixels = NULL;
        }
      /*  if(videoData->pitches){
            free(videoData->pitches);
        }*/
        if (videoData->pixelsY){
            free(videoData->pixelsY);
            videoData->pixelsY = NULL;
        }
        if (videoData->pixelsU){
            free(videoData->pixelsU);
            videoData->pixelsU = NULL;
        }
        if (videoData->pixelsV){
            free(videoData->pixelsV);
            videoData->pixelsV = NULL;
        }
        if(videoData->pixelsData){
            free(videoData->pixelsData);
            videoData->pixelsData = NULL;

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