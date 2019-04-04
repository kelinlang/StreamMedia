//
// Created by dengjun on 2019/3/26.
//

#include <malloc.h>
#include <android/display/cloudvoice_android_video_display.h>
#include "cloudvoice_push_stream_client.h"
static void setId(CloudVoicePushStreamClient self,char *id){
    if (self){
        self->rtmpClient->setId(self->rtmpClient,id);
    }
}

static void setParam(CloudVoicePushStreamClient self,CloudVoiceStreamParam param){
    if (self){
        self->streamParam = param;
        CloudVoiceRtmpParam  rtmpParam = self->rtmpClient->getParam(self->rtmpClient);
        rtmpParam->setUrl(rtmpParam,param->url);
    }
}
static CloudVoiceStreamParam getParam(CloudVoicePushStreamClient self){
    CloudVoiceStreamParam streamParam = NULL;
    if (self){
        streamParam = self->streamParam;
    }
    return streamParam;
}

static void setMediaStatusCallback(CloudVoicePushStreamClient self, MediaStatusCallback mediaStatusCallback){
    if (self){
        self->rtmpClient->setMediaStatusCallback(self->rtmpClient,mediaStatusCallback);
    }
}
static void setMediaDataCallback(CloudVoicePushStreamClient self, MediaDataCallback mediaDataCallback){
    if (self){
        self->rtmpClient->setMediaDataCallback(self->rtmpClient,mediaDataCallback);
    }
}

static void sendData(CloudVoicePushStreamClient self, CloudVoiceAVPacket srcPackect){
    if (self){
        self->rtmpClient->sendData(self->rtmpClient,srcPackect);
    }
}

static void start(CloudVoicePushStreamClient self){
    if (self){
        self->rtmpClient->start(self->rtmpClient);
    }
}
static void stop(CloudVoicePushStreamClient self){
    if (self){
        self->rtmpClient->stop(self->rtmpClient);
    }
}

static void destroy(CloudVoicePushStreamClient self){
    if (self){
        self->rtmpClient->destroy(self->rtmpClient);
        self->rtmpClient = NULL;
        free(self);
    }
}

CloudVoicePushStreamClient cloudVoiceCreatePushStreamClient(){
    CloudVoicePushStreamClient  pushStreamClient = (CloudVoicePushStreamClient)malloc(sizeof(CloudVoicePushStreamClient_));
    if (pushStreamClient){
        pushStreamClient->getParam = getParam;
        pushStreamClient->setParam = setParam;
        pushStreamClient->setMediaStatusCallback = setMediaStatusCallback;
        pushStreamClient->setMediaDataCallback = setMediaDataCallback;
        pushStreamClient->sendData = sendData;
        pushStreamClient->start = start;
        pushStreamClient->stop = stop;
        pushStreamClient->destroy = destroy;

        CloudVoiceRtmpClient rtmpClient = cloudVoiceCreateRtmpClient();
        rtmpClient->getParam(rtmpClient)->clientType = PUSH;
        pushStreamClient->rtmpClient = rtmpClient;


//        pushStreamClient->streamParam = cloudVoiceCreateStreamParam();
    }
}