//
// Created by dengjun on 2019/3/21.
//

#ifndef VOICELIBDEMO_CLOUDVOICE_CODEC_INTERFACE_H
#define VOICELIBDEMO_CLOUDVOICE_CODEC_INTERFACE_H

#include <library/media/model/cloudvoice_audio_video_model.h>
#include "cloudvoice_media_callback.h"

typedef enum
{   CODECTYPE_ENCODE_H264= 0,
    CODECTYPE_DECODE_H264,
} CloudVoiceCodecType;

typedef struct CloudVoiceCodecParam_ * CloudVoiceCodecParam;
typedef struct CloudVoiceCodecParam_{
    int codecType;
    int videoDataWidth;
    int videoDataHeight;
    int gravity;
    int videoBitrate;
    int videoFramerate;


}CloudVoiceCodecParam_;

CloudVoiceCodecParam cloudVoiceCreateCodecParam();
void cloudVoiceDestroyCodecParam(CloudVoiceCodecParam codecParam);


typedef struct CloudVoiceCodecOpaque_* CloudVoiceCodecOpaque;
//用户回调队列的数据，clear的时候不知道具体类型，需要回调给外面free具体数据
typedef struct CloudVoiceCodec_ * CloudVoiceCodec;

typedef struct CloudVoiceCodec_{
    CloudVoiceCodecOpaque codecOpaque;
    void *externHandle;

    void (*setParam)(CloudVoiceCodec self,CloudVoiceCodecParam codecParam);
    CloudVoiceCodecParam (*getParam)(CloudVoiceCodec codec);

    void (*setMediaStatusCallback)(CloudVoiceCodec self, MediaStatusCallback mediaStatusCallback);
    void (*setMediaDataCallback)(CloudVoiceCodec self, MediaDataCallback mediaDataCallback);

    void (*addData)(CloudVoiceCodec self, CloudVoiceAVPacket srcPackect);


    int (*transform)(CloudVoiceCodec self, void *srcPackect,void *dstPacket );


    void (*start)(CloudVoiceCodec self);
    void (*stop)(CloudVoiceCodec self);
    void (*destroy)(CloudVoiceCodec self);

}CloudVoiceCodec_;





#endif //VOICELIBDEMO_CLOUDVOICE_CODEC_INTERFACE_H
