//
// Created by dengjun on 2018/12/5.
//

#ifndef STREAMMEDIADEMO_SM_AUDIO_DATA_H
#define STREAMMEDIADEMO_SM_AUDIO_DATA_H

typedef struct SmAudioData{
    unsigned short *data;
}SmAudioData;

typedef struct SmAudioParam{
    int sampleRate;
    int channels;
    int mode;


}SmAudioParam;

SmAudioParam* CreateAudioParam(int sampleRate);


typedef void(*SmAudioDataCallback)(SmAudioData *audioData, void * pContext);

typedef void(*SmAudioStatusCallback)(int status, char* statusInfo, void * pContext);


#endif //STREAMMEDIADEMO_SM_AUDIO_DATA_H
