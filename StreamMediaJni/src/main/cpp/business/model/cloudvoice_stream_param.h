//
// Created by dengjun on 2019/3/26.
//

#ifndef VOICELIBDEMO_CLOUDVOICE_STREAM_PARAM_H
#define VOICELIBDEMO_CLOUDVOICE_STREAM_PARAM_H

typedef struct CloudVoiceStreamParam_* CloudVoiceStreamParam;
typedef struct CloudVoiceStreamParam_{
    int type;

    char *url;
}CloudVoiceStreamParam_;

CloudVoiceStreamParam cloudVoiceCreateStreamParam();
void  cloudVoiceDestroyStreamParam(CloudVoiceStreamParam StreamParam);



#endif //VOICELIBDEMO_CLOUDVOICE_STREAM_PARAM_H
