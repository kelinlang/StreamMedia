//
// Created by dengjun on 2019/3/22.
//

#ifndef VOICELIBDEMO_CLOUDVOICE_RTMP_CLIENT_H
#define VOICELIBDEMO_CLOUDVOICE_RTMP_CLIENT_H

#include <library/media/cloudvoice_media_callback.h>
#include <library/media/model/cloudvoice_audio_video_model.h>
typedef enum
{   PUSH = 0,
    PULL
} CloudVoiceRtmpClientType;

typedef struct CloudVoiceRtmpParam_* CloudVoiceRtmpParam;
typedef struct CloudVoiceRtmpParam_{
    int clientType;

    char *url;

    void(*setUrl)(CloudVoiceRtmpParam self,char *url);


}CloudVoiceRtmpParam_;

CloudVoiceRtmpParam cloudVoiceCreateRtmpParam();
void  cloudVoiceDestroyRtmpParam(CloudVoiceRtmpParam rtmpParam);



typedef struct CloudVoiceRtmpClientOpaque_* CloudVoiceRtmpClientOpaque;

typedef struct CloudVoiceRtmpClient_ * CloudVoiceRtmpClient;

typedef struct CloudVoiceRtmpClient_{
    CloudVoiceRtmpClientOpaque rtmpClientOpaque;
    void *externHandle;//外部引用传进入，回调的时候需要用到
    char * id;


    void (*setId)(CloudVoiceRtmpClient self,char *id);

    void (*setParam)(CloudVoiceRtmpClient self,CloudVoiceRtmpParam param);
    CloudVoiceRtmpParam (*getParam)(CloudVoiceRtmpClient display);

    void (*setMediaStatusCallback)(CloudVoiceRtmpClient self, MediaStatusCallback mediaStatusCallback);
    void (*setMediaDataCallback)(CloudVoiceRtmpClient self, MediaDataCallback mediaDataCallback);

    void (*sendData)(CloudVoiceRtmpClient self, CloudVoiceAVPacket srcPackect);



    void (*start)(CloudVoiceRtmpClient self);
    void (*stop)(CloudVoiceRtmpClient self);

    void (*destroy)(CloudVoiceRtmpClient self);

}CloudVoiceRtmpClient_;


CloudVoiceRtmpClient cloudVoiceCreateRtmpClient();

#endif //VOICELIBDEMO_CLOUDVOICE_RTMP_CLIENT_H
