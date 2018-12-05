//
// Created by dengjun on 2018/12/5.
//

#ifndef STREAMMEDIADEMO_SM_AUDIO_RECORD_H
#define STREAMMEDIADEMO_SM_AUDIO_RECORD_H

#include "sm_audio_data.h"
#include  <stdint.h>
#include  <stdio.h>
#include <malloc.h>

typedef  struct SmAudioRecord_ *SmAudioRecord;

//具体实现相关
typedef struct SmAudioRecordOpaque_* SmAudioRecordOpaque;

struct SmAudioRecord_{
    void (*SetAudioParam)(SmAudioRecord audioRecord,SmAudioParam *audioParam);
    SmAudioParam * (*GetAudioParam)(SmAudioRecord audioRecord);

    void * pContext;

    SmAudioRecordOpaque smAudioRecordOpaque;

    void (*SetAudioDataCallback)(SmAudioRecord audioRecord,SmAudioDataCallback audioDataCallback);
    void (*SetAudioStatusCallback)(SmAudioRecord audioRecord,SmAudioStatusCallback audioStatusCallback);

    void (*Start)(SmAudioRecord audioRecord);
    void (*Stop)(SmAudioRecord audioRecord);

    void (*destroy)(SmAudioRecord audioRecord);
}SmAudioRecord_;



SmAudioRecord  CreateAudioRecord(int type);




#endif //STREAMMEDIADEMO_SM_AUDIO_RECORD_H
