//
// Created by dengjun on 2019/3/26.
//

#ifndef VOICELIBDEMO_CLOUDVOICE_PULL_STREAM_CLIENT_H
#define VOICELIBDEMO_CLOUDVOICE_PULL_STREAM_CLIENT_H

#include <business/rtmp/cloudvoice_rtmp_client.h>
#include <android/display/cloudvoice_android_video_display.h>
#include <business/model/cloudvoice_player_param.h>
#include <library/media/cloudvoice_codec_interface.h>

typedef struct CloudVoicePullStreamPlayer_* CloudVoicePullStreamPlayer;
typedef struct CloudVoicePullStreamPlayer_{
    CloudVoicePlayerParam playerParam;
    CloudVoiceRtmpClient rtmpClient;
    CloudVoiceCodec decodec;

    CloudVoiceDisplay display;
    
    void (*setId)(CloudVoicePullStreamPlayer self,char *id);

    void (*setParam)(CloudVoicePullStreamPlayer self,CloudVoicePlayerParam param);
    CloudVoicePlayerParam (*getParam)(CloudVoicePullStreamPlayer self);

    void (*setMediaStatusCallback)(CloudVoicePullStreamPlayer self, MediaStatusCallback mediaStatusCallback);
    void (*setMediaDataCallback)(CloudVoicePullStreamPlayer self, MediaDataCallback mediaDataCallback);

    void (*sendData)(CloudVoicePullStreamPlayer self, CloudVoiceAVPacket srcPackect);

    void (*start)(CloudVoicePullStreamPlayer self);
    void (*stop)(CloudVoicePullStreamPlayer self);

    void (*destroy)(CloudVoicePullStreamPlayer self);




}CloudVoicePullStreamPlayer_;

CloudVoicePullStreamPlayer cloudVoiceCreatePullStreamPlayer();

#endif //VOICELIBDEMO_CLOUDVOICE_PULL_STREAM_CLIENT_H
