//
// Created by dengjun on 2018/12/5.
//

#include "sm_audio_record.h"

struct SmAudioRecordOpaque_{

};

static void SetAudioParam(SmAudioRecord audioRecord,SmAudioParam *audioParam){
    if (audioRecord != NULL && audioRecord->SetAudioParam != NULL){
        audioRecord->SetAudioParam(audioRecord,audioParam);
    }
}


static void SetAudioDataCallback(SmAudioRecord audioRecord,SmAudioDataCallback audioDataCallback){
    if (audioRecord != NULL && audioRecord->SetAudioDataCallback != NULL){
        audioRecord->SetAudioDataCallback(audioRecord,audioDataCallback);
    }
}
static void SetAudioStatusCallback(SmAudioRecord audioRecord,SmAudioStatusCallback audioStatusCallback){
    if (audioRecord != NULL && audioRecord->SetAudioStatusCallback != NULL){
        audioRecord->SetAudioStatusCallback(audioRecord,audioStatusCallback);
    }
}


static void Start(SmAudioRecord audioRecord){
    if (audioRecord != NULL){

    }
}

static void Stop(SmAudioRecord audioRecord){
    if (audioRecord != NULL){

    }
}


SmAudioRecord  CreateAudioRecord(int type){
    SmAudioRecord smAudioRecord = (SmAudioRecord)malloc(sizeof(SmAudioRecord));
    if (smAudioRecord != NULL){
        smAudioRecord->SetAudioParam = SetAudioParam;
        smAudioRecord->SetAudioDataCallback = SetAudioDataCallback;
        smAudioRecord->Start = Start;
        smAudioRecord->Stop = Stop;

        return smAudioRecord;
    }
    return NULL;
}