//
// Created by dengjun on 2019/1/18.
//

#include <stdint.h>
#include <media/NdkMediaCodec.h>
#include <jni.h>
#include "../model/sm_data_callback_interface.h"

#ifndef STREAMMEDIADEMO_SM_MEDIACODEC_TEST_H
#define STREAMMEDIADEMO_SM_MEDIACODEC_TEST_H

struct SmMediaCodec_;

typedef struct SmMediaCodec_* SmMediaCodec;

SmMediaCodec smCreateMediaCodec();
void smMediaCodecSetDataCallback(SmMediaCodec mediaCodec, DataCallback dataCallback);
void smMediaCodecstart(SmMediaCodec mediaCodec);
void smMediaCodecstop(SmMediaCodec mediaCodec);
void smMediaCodecRelease(SmMediaCodec mediaCodec);

void smMediaCodecAddData(SmMediaCodec mediaCodec, uint8_t *h264Data,int h264DataLen);

#endif //STREAMMEDIADEMO_SM_MEDIACODEC_TEST_H
