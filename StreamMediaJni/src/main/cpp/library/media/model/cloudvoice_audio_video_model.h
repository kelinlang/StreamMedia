//
// Created by dengjun on 2019/3/21.
//

#ifndef VOICELIBDEMO_CLOUDVOICE_AUDIO_VIDEO_MODEL_H
#define VOICELIBDEMO_CLOUDVOICE_AUDIO_VIDEO_MODEL_H


#include <stdint.h>

#define SM_ID_MAX_LEN 32

typedef enum
{   VDIEO_FORMAT_H264 = 0,
    VDIEO_FORMAT_YUV,
    VDIEO_FORMAT_RGB,
    VDIEO_FORMAT_H264_SPS_PPS
} CloudVoicePacketFormat;

typedef struct CloudVoiceAVPacket_ * CloudVoiceAVPacket;

typedef struct CloudVoiceAVPacket_{
    int packetFormat;
    int64_t presentTimeStamp;

    int64_t decodeTimeStamp;

    int64_t timeStamp;

    int dataLen;
    int startPos;
    uint8_t * data;

    int frameType;//帧类型，关键帧，非关键帧


    uint8_t * sps;
    int spsLen;

    uint8_t * pps;
    int ppsLen;
} CloudVoiceAVPacket_;

CloudVoiceAVPacket cloudVoiceCreateAVPackect();
void cloudVoiceDestroyAVPackect(CloudVoiceAVPacket avPackect);


typedef struct CloudVoiceVideoParam_ * CloudVoiceVideoParam;

typedef struct CloudVoiceVideoParam_{
    int viewWidth;
    int viewHeight;
    int gravity;

    unsigned int dataFormat;
    unsigned int displayFormat;


    int videoWidth;
    int videoHeight;

    float *matrix;
    int matrixLen;
}CloudVoiceVideoParam_;

CloudVoiceVideoParam cloudVoiceCreateVideoParam();
void cloudVoiceDestroyVideoParam(CloudVoiceVideoParam videoParam);

typedef struct CloudVoiceDisplayParam_ * CloudVoiceDisplayParam;

typedef struct CloudVoiceDisplayParam_{
    int viewWidth;
    int viewHeight;
    int gravity;

    unsigned int dataFormat;
    unsigned int displayFormat;


    int videoWidth;
    int videoHeight;

    float *matrix;
    int matrixLen;
}CloudVoiceDisplayParam_;

CloudVoiceDisplayParam cloudVoiceCreateDisplayParam();
void cloudVoiceDestroyDisplayParam(CloudVoiceDisplayParam displayParam);

#endif //VOICELIBDEMO_CLOUDVOICE_AUDIO_VIDEO_MODEL_H
