//
// Created by dengjun on 2019/3/26.
//

#include <malloc.h>
#include "cloudvoice_media_manager_cpp.h"


extern "C" {
#include <business/client/cloudvoice_pull_stream_player.h>
#include <business/client/cloudvoice_push_stream_client.h>
#include <library/CommonLib/log/cloudvoice_log.h>
#include "cloudvoice_media_manager_cpp.h"
}



void CloudVoiceMediaManager::init() {
    if(initFlag == 0){
        initFlag = 1;
    }
}

void CloudVoiceMediaManager::release() {
    if(initFlag == 1){
        initFlag = 0;
    }
}

void CloudVoiceMediaManager::setMediaStatusCallback() {

}

void CloudVoiceMediaManager::setParam(CloudVoiceMediaParam mediaParam) {

}

void CloudVoiceMediaManager::resume() {

}

void CloudVoiceMediaManager::pause() {

}

void CloudVoiceMediaManager::createPlayer(char *id) {
    if (initFlag == 1){
        CloudVoicePullStreamPlayer player = cloudVoiceCreatePullStreamPlayer();
        pullStreamPlayerMap.insert(pair<char *, CloudVoicePullStreamPlayer>(id, player));
    }
}

void CloudVoiceMediaManager::setVideoSurface(char *id, ANativeWindow *nativeWindow) {
    if (initFlag == 1){
        CloudVoicePullStreamPlayer player = pullStreamPlayerMap.at(id);
        if (player){
            player->display->setVideoSurface(player->display,nativeWindow);
        }
    }
}

void CloudVoiceMediaManager::setVideoMatrix(char *id, float *matrix) {
    if (initFlag == 1){
        CloudVoicePullStreamPlayer player = pullStreamPlayerMap.at(id);
        if (player){
            player->display->setVideoMatrix(player->display,matrix);
        }
    }
}

void CloudVoiceMediaManager::setPlayerParam(char *id, CloudVoicePlayerParam playerParam) {
    if (initFlag == 1){
        CloudVoicePullStreamPlayer player = pullStreamPlayerMap.at(id);
        if (player){
            player->setParam(player,playerParam);
        }
    }
}

CloudVoicePlayerParam CloudVoiceMediaManager::getPlayerParam(char *id) {
    if (initFlag == 1){
        CloudVoicePullStreamPlayer player = pullStreamPlayerMap.at(id);
        if (player){
            return player->getParam(player);
        }
    }
    return nullptr;
}

void CloudVoiceMediaManager::startPlay(char *id) {
    if (initFlag == 1){
        CloudVoicePullStreamPlayer player = pullStreamPlayerMap.at(id);
        if (player){
            player->start(player);
        }
    }
}

void CloudVoiceMediaManager::stopPlay(char *id) {
    if (initFlag == 1){
        CloudVoicePullStreamPlayer player = pullStreamPlayerMap.at(id);
        if (player){
            player->stop(player);
        }
    }
}

void CloudVoiceMediaManager::createPushClient(char *id) {
    if (initFlag == 1){
        CloudVoicePushStreamClient pushStreamClient = cloudVoiceCreatePushStreamClient();
        pushStreamClientMap.insert(pair<char *, CloudVoicePushStreamClient>(id, pushStreamClient));
        cloudVoiceLogD("%s pushStreamClient  create addr :%d", id,pushStreamClient);
    }
}

void CloudVoiceMediaManager::setPushStreamParam(char *id, CloudVoiceStreamParam streamParam) {
    if (initFlag == 1){
        CloudVoicePushStreamClient pushStreamClient = pushStreamClientMap.at(id);
        if (pushStreamClient){
            pushStreamClient->setParam(pushStreamClient,streamParam);
        }
    }
}

void CloudVoiceMediaManager::startPush(char *id) {
    if (initFlag == 1){
        CloudVoicePushStreamClient pushStreamClient = pushStreamClientMap.at(id);
        if (pushStreamClient){
            pushStreamClient->start(pushStreamClient);
        }
    }
}

void CloudVoiceMediaManager::stopPush(char *id) {
    if (initFlag == 1){
        CloudVoicePushStreamClient pushStreamClient = pushStreamClientMap.at(id);
        if (pushStreamClient){
            pushStreamClient->stop(pushStreamClient);
        }
    }
}

void CloudVoiceMediaManager::sendVideoData(char *id, CloudVoiceAVPacket avPacket) {
    if (initFlag == 1){
        CloudVoicePushStreamClient pushStreamClient = pushStreamClientMap.at(id);
        //推流到服务器
        /*CloudVoicePushStreamClient pushStreamClient = (CloudVoicePushStreamClient)map_get(&mediaManager->pushStreamClientMap,id);
        if (pushStreamClient){
            pushStreamClient->sendData(pushStreamClient,avPacket);
        }*/


        //直接送给播放器，界面播放
        CloudVoicePullStreamPlayer player = pullStreamPlayerMap.at(id);
        if (player){
            player->sendData(player,avPacket);
        }
    }
}

void CloudVoiceMediaManager::sendAudioData(char *id, CloudVoiceAVPacket avPacket) {
    if (initFlag == 1){

    }
}




