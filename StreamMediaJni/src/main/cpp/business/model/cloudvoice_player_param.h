//
// Created by dengjun on 2019/3/28.
//

#ifndef VOICELIBDEMO_CLOUDVOICE_PLAYER_PARAM_H
#define VOICELIBDEMO_CLOUDVOICE_PLAYER_PARAM_H

typedef struct CloudVoicePlayerParam_ * CloudVoicePlayerParam;

typedef struct CloudVoicePlayerParam_{
    //显示窗口
    int viewWidth;
    int viewHeight;
    int gravity;

    unsigned int dataFormat;
    unsigned int displayFormat;


    //视频数据
    int videoWidth;
    int videoHeight;

    float *matrix;
    int matrixLen;

    char *url;


    void(*setUrl)(CloudVoicePlayerParam self,char *url);
}CloudVoicePlayerParam_;

CloudVoicePlayerParam cloudVoiceCreatePlayerParam();
void cloudVoiceDestroyPlayerParam(CloudVoicePlayerParam playerParam);

#endif //VOICELIBDEMO_CLOUDVOICE_PLAYER_PARAM_H
