//
// Created by dengjun on 2019/3/26.
//

#ifndef VOICELIBDEMO_CLOUDVOICE_MEDIA_PARAMS_H
#define VOICELIBDEMO_CLOUDVOICE_MEDIA_PARAMS_H
typedef struct CloudVoiceMediaParam_* CloudVoiceMediaParam;
typedef struct CloudVoiceMediaParam_{
    int type;

    char *url;
}CloudVoiceMediaParam_;

CloudVoiceMediaParam cloudVoiceCreateMediaParam();
void  cloudVoiceDestroyMediaParam(CloudVoiceMediaParam MediaParam);


#endif //VOICELIBDEMO_CLOUDVOICE_MEDIA_PARAMS_H
