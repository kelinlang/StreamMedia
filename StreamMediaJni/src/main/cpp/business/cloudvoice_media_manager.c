//
// Created by dengjun on 2019/3/26.
//

#include <malloc.h>
#include <business/client/cloudvoice_pull_stream_player.h>
#include <business/client/cloudvoice_push_stream_client.h>
#include "cloudvoice_media_manager.h"


static void init(CloudVoiceMediaManager mediaManager){
    if(mediaManager && mediaManager->initFlag == 0){
        map_init(&mediaManager->pullStreamPlayerMap);
        map_init(&mediaManager->pushStreamClientMap);
    }
}
static void release(CloudVoiceMediaManager mediaManager){
    if(mediaManager && mediaManager->initFlag == 1){
        map_deinit_(&mediaManager->pullStreamPlayerMap);
        map_deinit_(&mediaManager->pushStreamClientMap);
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
        map_set_(&mediaManager->pullStreamPlayerMap,id,player, sizeof(CloudVoicePullStreamPlayer));
    }
}
 
static void setVideoSurface(CloudVoiceMediaManager mediaManager,char *id, ANativeWindow * nativeWindow){
    if (mediaManager && mediaManager->initFlag == 1 && id){
        CloudVoicePullStreamPlayer player = (CloudVoicePullStreamPlayer)map_get_(&mediaManager->pullStreamPlayerMap,id);
        if (player){
            player->display->setVideoSurface(player->display,nativeWindow);
        }
    }
}

static void setVideoMatrix(CloudVoiceMediaManager mediaManager,char *id,float* matrix ){
    if (mediaManager && mediaManager->initFlag == 1 && id){
        CloudVoicePullStreamPlayer player = (CloudVoicePullStreamPlayer)map_get_(&mediaManager->pullStreamPlayerMap,id);
        if (player){
            player->display->setVideoMatrix(player->display,matrix);
        }
    }
}


static void setPlayerParam(CloudVoiceMediaManager mediaManager, char *id, CloudVoicePlayerParam playerParam){
    if (mediaManager && mediaManager->initFlag == 1 && id){
        CloudVoicePullStreamPlayer player = (CloudVoicePullStreamPlayer)map_get_(&mediaManager->pullStreamPlayerMap,id);
        if (player){
            player->setParam(player,playerParam);
        }
    }
}

static CloudVoicePlayerParam getPlayerParam(CloudVoiceMediaManager mediaManager, char *id){
    if (mediaManager && mediaManager->initFlag == 1 && id){
        CloudVoicePullStreamPlayer player = (CloudVoicePullStreamPlayer)map_get_(&mediaManager->pullStreamPlayerMap,id);
        if (player){
            return player->getParam(player;
        }
    }
}

static void startPlay(CloudVoiceMediaManager mediaManager, char *id){
    if (mediaManager && mediaManager->initFlag == 1 && id){
        CloudVoicePullStreamPlayer player = (CloudVoicePullStreamPlayer)map_get_(&mediaManager->pullStreamPlayerMap,id);
        if (player){
            player->start(player);
        }
    }
}


static void stopPlay(CloudVoiceMediaManager mediaManager, char *id){
    if (mediaManager && mediaManager->initFlag == 1 && id){
        CloudVoicePullStreamPlayer player = (CloudVoicePullStreamPlayer)map_get_(&mediaManager->pullStreamPlayerMap,id);
        if (player){
            player->stop(player);
        }
    }
}


static void createPushClient(CloudVoiceMediaManager mediaManager, char*id){
    if (mediaManager && mediaManager->initFlag == 1){
        CloudVoicePushStreamClient pushStreamClient = cloudVoiceCreatePushStreamClient();
        map_set_(&mediaManager->pushStreamClientMap,id,pushStreamClient, sizeof(CloudVoicePushStreamClient));
    }
}

static void setPushStreamParam(CloudVoiceMediaManager mediaManager,char*id,CloudVoiceStreamParam streamParam){
    if (mediaManager && mediaManager->initFlag == 1 && id){
        CloudVoicePushStreamClient pushStreamClient = (CloudVoicePushStreamClient)map_get_(&mediaManager->pushStreamClientMap,id);
        if (pushStreamClient){
            pushStreamClient->setParam(pushStreamClient,streamParam);
        }
    }
}

static void startPush(CloudVoiceMediaManager mediaManager,char*id){
    if (mediaManager && mediaManager->initFlag == 1 && id){
        CloudVoicePushStreamClient pushStreamClient = (CloudVoicePushStreamClient)map_get_(&mediaManager->pushStreamClientMap,id);
        if (pushStreamClient){
            pushStreamClient->start(pushStreamClient);
        }
    }
}

static void stopPush(CloudVoiceMediaManager mediaManager,char*id){
    if (mediaManager && mediaManager->initFlag == 1 && id){
        CloudVoicePushStreamClient pushStreamClient = (CloudVoicePushStreamClient)map_get_(&mediaManager->pushStreamClientMap,id);
        if (pushStreamClient){
            pushStreamClient->stop(pushStreamClient);
        }
    }
}
 
static void sendVideoData(CloudVoiceMediaManager mediaManager,char * id,CloudVoiceAVPacket avPacket){
    if (mediaManager && mediaManager->initFlag == 1 && id){
        CloudVoicePushStreamClient pushStreamClient = (CloudVoicePushStreamClient)map_get_(&mediaManager->pushStreamClientMap,id);
        if (pushStreamClient){
            pushStreamClient->sendData(pushStreamClient,avPacket);
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