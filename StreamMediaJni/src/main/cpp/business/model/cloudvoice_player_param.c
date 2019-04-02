//
// Created by dengjun on 2019/3/28.
//

#include <malloc.h>
#include <memory.h>
#include <library/CommonLib/utils/cloudvoice_utils.h>
#include "cloudvoice_player_param.h"

static void setUrl(CloudVoicePlayerParam playerParam, char *url){
    if (playerParam && url){
        cloudVoiceStringCopy(&playerParam->url,url);
    }
}


CloudVoicePlayerParam cloudVoiceCreatePlayerParam(){
    CloudVoicePlayerParam playerParam = (CloudVoicePlayerParam)malloc(sizeof(CloudVoicePlayerParam_));
    if(playerParam){
        playerParam->viewWidth = 0;
        playerParam->viewHeight = 0;
        playerParam->videoWidth = 0;
        playerParam->videoHeight = 0;
        playerParam->dataFormat = 0;
        playerParam->displayFormat = 0;
        playerParam->matrixLen = 0;
        playerParam->matrix = NULL;

        playerParam->setUrl = setUrl;
    }
    return playerParam;
}
void cloudVoiceDestroyPlayerParam(CloudVoicePlayerParam playerParam){
    if(playerParam){
        if(playerParam->matrix){
            free(playerParam->matrix);
        }
        if (playerParam->url){
            free(playerParam->url);
        }
        free(playerParam);
    }
}