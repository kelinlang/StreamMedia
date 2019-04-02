//
// Created by dengjun on 2019/3/26.
//

#ifndef VOICELIBDEMO_CLOUDVOICE_PUSH_STREAM_CLIENT_H
#define VOICELIBDEMO_CLOUDVOICE_PUSH_STREAM_CLIENT_H

#include <business/rtmp/cloudvoice_rtmp_client.h>

typedef struct CloudVoicePushStreamClient_* CloudVoicePushStreamClient;
typedef struct CloudVoicePushStreamClient_{
    CloudVoiceRtmpClient rtmpClient;
    void *externHandle;

    void (*setId)(CloudVoicePushStreamClient self,char *id);

    void (*setParam)(CloudVoicePushStreamClient self,CloudVoiceRtmpParam param);
    CloudVoiceRtmpParam (*getParam)(CloudVoicePushStreamClient self);

    void (*setMediaStatusCallback)(CloudVoicePushStreamClient self, MediaStatusCallback mediaStatusCallback);
    void (*setMediaDataCallback)(CloudVoicePushStreamClient self, MediaDataCallback mediaDataCallback);

    void (*sendData)(CloudVoicePushStreamClient self, CloudVoiceAVPacket srcPackect);

    void (*start)(CloudVoicePushStreamClient self);
    void (*stop)(CloudVoicePushStreamClient self);

    void (*destroy)(CloudVoicePushStreamClient self);
}CloudVoicePushStreamClient_;


CloudVoicePushStreamClient cloudVoiceCreatePushStreamClient();


#endif //VOICELIBDEMO_CLOUDVOICE_PUSH_STREAM_CLIENT_H
