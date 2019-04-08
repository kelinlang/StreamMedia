//
// Created by dengjun on 2019/3/26.
//

#include <malloc.h>
#include <business/client/cloudvoice_pull_stream_player.h>
#include <business/client/cloudvoice_push_stream_client.h>
#include <library/CommonLib/log/cloudvoice_log.h>
#include "cloudvoice_media_manager.h"



static void init(CloudVoiceMediaManager mediaManager){
    if(mediaManager && mediaManager->initFlag == 0){
        map_init(&mediaManager->pullStreamPlayerMap);
        map_init(&mediaManager->pushStreamClientMap);
        mediaManager->initFlag = 1;
    }
}
static void release(CloudVoiceMediaManager mediaManager){
    if(mediaManager && mediaManager->initFlag == 1){
        map_deinit(&mediaManager->pullStreamPlayerMap);
        map_deinit(&mediaManager->pushStreamClientMap);
        mediaManager->initFlag = 0;
    }
}
 
static void setMediaStatusCallback(CloudVoiceMediaManager mediaManager){
    
}

static void setParam(CloudVoiceMediaManager mediaManager,CloudVoiceMediaParam mediaParam){
    
}
 
static void resume(CloudVoiceMediaManager mediaManager){

}

static void pause(CloudVoiceMediaManager mediaManager){
    
}
static void createPlayer(CloudVoiceMediaManager mediaManager, char*id){
    if (mediaManager && mediaManager->initFlag == 1){
        CloudVoicePullStreamPlayer player = cloudVoiceCreatePullStreamPlayer();
        map_set(&mediaManager->pullStreamPlayerMap,id,player);
    }
}
 
static void setVideoSurface(CloudVoiceMediaManager mediaManager,char *id, ANativeWindow * nativeWindow){
    if (mediaManager && mediaManager->initFlag == 1 && id){
        CloudVoicePullStreamPlayer player = (CloudVoicePullStreamPlayer)map_get(&mediaManager->pullStreamPlayerMap,id);
        if (player){
            player->display->setVideoSurface(player->display,nativeWindow);
        }
    }
}

static void setVideoMatrix(CloudVoiceMediaManager mediaManager,char *id,float* matrix ){
    if (mediaManager && mediaManager->initFlag == 1 && id){
        CloudVoicePullStreamPlayer player = (CloudVoicePullStreamPlayer)map_get(&mediaManager->pullStreamPlayerMap,id);
        if (player){
            player->display->setVideoMatrix(player->display,matrix);
        }
    }
}


static void setPlayerParam(CloudVoiceMediaManager mediaManager, char *id, CloudVoicePlayerParam playerParam){
    if (mediaManager && mediaManager->initFlag == 1 && id){
        CloudVoicePullStreamPlayer player = (CloudVoicePullStreamPlayer)map_get(&mediaManager->pullStreamPlayerMap,id);
        if (player){
            player->setParam(player,playerParam);
        }
    }
}

static CloudVoicePlayerParam getPlayerParam(CloudVoiceMediaManager mediaManager, char *id){
    if (mediaManager && mediaManager->initFlag == 1 && id){
        CloudVoicePullStreamPlayer player = (CloudVoicePullStreamPlayer)map_get(&mediaManager->pullStreamPlayerMap,id);
        if (player){
            return player->getParam(player);
        }
    }
    return NULL;
}

static void startPlay(CloudVoiceMediaManager mediaManager, char *id){
    if (mediaManager && mediaManager->initFlag == 1 && id){
        CloudVoicePullStreamPlayer player = (CloudVoicePullStreamPlayer)map_get(&mediaManager->pullStreamPlayerMap,id);
        if (player){
            player->start(player);
        }
    }
}


static void stopPlay(CloudVoiceMediaManager mediaManager, char *id){
    if (mediaManager && mediaManager->initFlag == 1 && id){
        CloudVoicePullStreamPlayer player = (CloudVoicePullStreamPlayer)map_get(&mediaManager->pullStreamPlayerMap,id);
        if (player){
            player->stop(player);
        }
    }
}


static void createPushClient(CloudVoiceMediaManager mediaManager, char*id){
    if (mediaManager && mediaManager->initFlag == 1){
        CloudVoicePushStreamClient pushStreamClient = cloudVoiceCreatePushStreamClient();
        cloudVoiceLogD("%s pushStreamClient  create addr :%d", id,pushStreamClient);
        map_set(&mediaManager->pushStreamClientMap,id,pushStreamClient);
    }
}

static void setPushStreamParam(CloudVoiceMediaManager mediaManager,char*id,CloudVoiceStreamParam streamParam){
    if (mediaManager && mediaManager->initFlag == 1 && id){
        CloudVoicePushStreamClient pushStreamClient = (CloudVoicePushStreamClient)map_get(&mediaManager->pushStreamClientMap,id);
        cloudVoiceLogD("%s pushStreamClient  map_get addr :%d",id, pushStreamClient);
        if (pushStreamClient){
            pushStreamClient->setParam(pushStreamClient,streamParam);
        }
    }
}

static void startPush(CloudVoiceMediaManager mediaManager,char*id){
    if (mediaManager && mediaManager->initFlag == 1 && id){
        CloudVoicePushStreamClient pushStreamClient = (CloudVoicePushStreamClient)map_get(&mediaManager->pushStreamClientMap,id);
        if (pushStreamClient){
            pushStreamClient->start(pushStreamClient);
        }
    }
}

static void stopPush(CloudVoiceMediaManager mediaManager,char*id){
    if (mediaManager && mediaManager->initFlag == 1 && id){
        CloudVoicePushStreamClient pushStreamClient = (CloudVoicePushStreamClient)map_get(&mediaManager->pushStreamClientMap,id);
        if (pushStreamClient){
            pushStreamClient->stop(pushStreamClient);
        }
    }
}
 
static void sendVideoData(CloudVoiceMediaManager mediaManager,char * id,CloudVoiceAVPacket avPacket){
    if (mediaManager && mediaManager->initFlag == 1 && id){
        //推流到服务器
        /*CloudVoicePushStreamClient pushStreamClient = (CloudVoicePushStreamClient)map_get(&mediaManager->pushStreamClientMap,id);
        if (pushStreamClient){
            pushStreamClient->sendData(pushStreamClient,avPacket);
        }*/


        //直接送给播放器，界面播放
        CloudVoicePullStreamPlayer player = (CloudVoicePullStreamPlayer)map_get(&mediaManager->pullStreamPlayerMap,id);
        if (player){
            player->sendData(player,avPacket);
        }
    }
}

static void sendAudioData(CloudVoiceMediaManager mediaManager,char * id,CloudVoiceAVPacket avPacket){
    
}

CloudVoiceMediaManager cloudVoiceCreateMediaManager(){
    CloudVoiceMediaManager mediaManager = (CloudVoiceMediaManager)malloc(sizeof(CloudVoiceMediaManager_));
    if (mediaManager){
        mediaManager->init = init;
        mediaManager->release = release;
        mediaManager->setMediaStatusCallback = setMediaStatusCallback;
        mediaManager->setParam = setParam;
        mediaManager->getPlayerParam = getPlayerParam;
        mediaManager->resume = resume;
        mediaManager->pause = pause;
        mediaManager->createPlayer = createPlayer;
        mediaManager->setVideoSurface = setVideoSurface;
        mediaManager->setVideoMatrix = setVideoMatrix;
        mediaManager->setPlayerParam = setPlayerParam;
        mediaManager->startPlay = startPlay;
        mediaManager->stopPlay = stopPlay;
        mediaManager->createPushClient = createPushClient;
        mediaManager->setPushStreamParam = setPushStreamParam;
        mediaManager->startPush = startPush;
        mediaManager->stopPush = stopPush;
        mediaManager->sendVideoData = sendVideoData;
        mediaManager->sendAudioData = sendAudioData;

        mediaManager->initFlag = 0;
    }
    return mediaManager;
}