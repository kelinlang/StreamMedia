//
// Created by dengjun on 2019/3/26.
//

#include <malloc.h>
#include <android/codec/cloudvoide_android_video_codec.h>
#include <memory.h>
#include "cloudvoice_pull_stream_player.h"

//rtmp回调接收的数据
static void RtmpMediaDataCallback(void *externHandle,char *id, int type,  void * dataPacket,void *ext){
    CloudVoicePullStreamPlayer player = (CloudVoicePullStreamPlayer)externHandle;
    CloudVoiceAVPacket  avPacket = (CloudVoiceAVPacket)dataPacket;
    if(player){
        player->decodec->addData(player->decodec,avPacket);
    } else{
       cloudVoiceDestroyAVPackect(avPacket);
    }
}

static void RtmpMediaStatusCallback(void *externHandle,char *id, int  mediaStatus,void *ext){
    CloudVoicePullStreamPlayer player = (CloudVoicePullStreamPlayer)externHandle;
}

//解码器回调解码后的数据
static void DecodeMediaDataCallback(void *externHandle,char *id, int type,  void * dataPacket,void *ext){
    CloudVoicePullStreamPlayer player = (CloudVoicePullStreamPlayer)externHandle;
    CloudVoiceAVPacket  avPacket = (CloudVoiceAVPacket)dataPacket;
    if(player){
        player->display->addData(player->display,avPacket);
    } else{
        cloudVoiceDestroyAVPackect(avPacket);
    }
}




static void DecodecMediaStatusCallback(void *externHandle,char *id, int  mediaStatus,void *ext){
    CloudVoicePullStreamPlayer player = (CloudVoicePullStreamPlayer)externHandle;
}

static void DisplayMediaStatusCallback(void *externHandle,char *id, int  mediaStatus,void *ext){
    CloudVoicePullStreamPlayer player = (CloudVoicePullStreamPlayer)externHandle;
}


static void setId(CloudVoicePullStreamPlayer self,char *id){
    if (self){
        self->rtmpClient->setId(self->rtmpClient,id);
    }
}

static void setParam(CloudVoicePullStreamPlayer self,CloudVoicePlayerParam param){
    if (self){
        self->playerParam = param;

        CloudVoiceCodecParam  codecParam = self->decodec->getParam(self->decodec);
        codecParam->videoDataWidth = param->videoWidth;
        codecParam->videoDataHeight = param->videoHeight;

        CloudVoiceDisplayParam  displayParam = self->display->getParam(self->display);
        displayParam->videoWidth = param->videoWidth;
        displayParam->videoHeight = param->videoHeight;
        displayParam->matrix = (float*)malloc(param->matrixLen);
        memcpy(displayParam->matrix, param->matrix, param->matrixLen);



        CloudVoiceRtmpParam  rtmpParam = self->rtmpClient->getParam(self->rtmpClient);
        rtmpParam->setUrl(rtmpParam,param->url);
    }
}

static void setParamInter(CloudVoicePullStreamPlayer self){
    CloudVoicePlayerParam param = self->playerParam;

    CloudVoiceCodecParam  codecParam = self->decodec->getParam(self->decodec);
    codecParam->videoDataWidth = param->videoWidth;
    codecParam->videoDataHeight = param->videoHeight;

    CloudVoiceDisplayParam  displayParam = self->display->getParam(self->display);
    displayParam->videoWidth = param->videoWidth;
    displayParam->videoHeight = param->videoHeight;
    displayParam->matrix = (float*)malloc(param->matrixLen);
    memcpy(displayParam->matrix, param->matrix, param->matrixLen);



    CloudVoiceRtmpParam  rtmpParam = self->rtmpClient->getParam(self->rtmpClient);
    rtmpParam->setUrl(rtmpParam,param->url);
}

static CloudVoicePlayerParam getParam(CloudVoicePullStreamPlayer self){
    CloudVoicePlayerParam playerParam = NULL;
    if (self){
        playerParam = self->playerParam;
    }
    return playerParam;
}

static void setMediaStatusCallback(CloudVoicePullStreamPlayer self, MediaStatusCallback mediaStatusCallback){
    if (self){
        self->rtmpClient->setMediaStatusCallback(self->rtmpClient,mediaStatusCallback);
    }
}
static void setMediaDataCallback(CloudVoicePullStreamPlayer self, MediaDataCallback mediaDataCallback){
    if (self){
        self->rtmpClient->setMediaDataCallback(self->rtmpClient,mediaDataCallback);
    }
}

static void sendData(CloudVoicePullStreamPlayer self, CloudVoiceAVPacket srcPackect){
    if (self){
        self->rtmpClient->sendData(self->rtmpClient,srcPackect);
    }
}

static void start(CloudVoicePullStreamPlayer self){
    if (self){
        setParamInter(self);

        self->display->start(self->display);
        self->decodec->start(self->decodec);
        self->rtmpClient->start(self->rtmpClient);
    }
}
static void stop(CloudVoicePullStreamPlayer self){
    if (self){
        self->rtmpClient->stop(self->rtmpClient);
        self->decodec->stop(self->decodec);
        self->display->stop(self->display);
    }
}

static void destroy(CloudVoicePullStreamPlayer self){
    if (self){
        self->rtmpClient->destroy(self->rtmpClient);
        self->rtmpClient = NULL;

        self->decodec->destroy(self->decodec);
        self->decodec = NULL;

        self->display->destroy(self->display);
        self->display = NULL;

        cloudVoiceDestroyPlayerParam(self->playerParam);
        self->playerParam = NULL;

        free(self);
    }
}

CloudVoicePullStreamPlayer cloudVoiceCreatePullStreamPlayer(){
    CloudVoicePullStreamPlayer  player = (CloudVoicePullStreamPlayer)malloc(sizeof(CloudVoicePullStreamPlayer_));
    if (player){
        player->getParam = getParam;
        player->setParam = setParam;
        player->setMediaStatusCallback = setMediaStatusCallback;
        player->setMediaDataCallback = setMediaDataCallback;
        player->sendData = sendData;
        player->start = start;
        player->stop = stop;
        player->destroy = destroy;

        player->playerParam = cloudVoiceCreatePlayerParam();

        CloudVoiceRtmpClient rtmpClient = cloudVoiceCreateRtmpClient();
        rtmpClient->getParam(rtmpClient)->clientType = PULL;
        player->rtmpClient = rtmpClient;
        player-> rtmpClient->externHandle = player;
        player-> rtmpClient->setMediaStatusCallback(player-> rtmpClient,RtmpMediaStatusCallback);
        player-> rtmpClient->setMediaDataCallback(player-> rtmpClient,RtmpMediaDataCallback);


        player->display = cloudVoiceCreateAndroidVideoDisplay();
        player->display->externHandle = player;
        player-> display->setMediaStatusCallback(player-> display,DisplayMediaStatusCallback);

        player->decodec = cloudVoiceCreateAndroidVideoCodec(NULL);
        player->decodec->externHandle = player;
        player-> decodec->setMediaStatusCallback(player-> decodec,DecodecMediaStatusCallback);
        player-> decodec->setMediaDataCallback(player-> decodec,DecodeMediaDataCallback);

    }
    return player;
}