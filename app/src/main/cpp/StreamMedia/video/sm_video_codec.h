//
// Created by dengjun on 2018/11/16.
//

#ifndef STREAMMEDIADEMO_SM_VIDEO_CODEC_H
#define STREAMMEDIADEMO_SM_VIDEO_CODEC_H

#include <media/NdkMediaCodec.h>

typedef struct SmVideoCodec_ * SmVideoCodec;
typedef struct SmVideoCodec_{
    int encodeThreadRunFlag;
    pthread_t encodeThreadId;
    AMediaCodec *encode;
}SmVideoCodec_;




#endif //STREAMMEDIADEMO_SM_VIDEO_CODEC_H
