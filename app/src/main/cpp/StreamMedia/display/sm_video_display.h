//
// Created by dengjun on 2018/11/16.
//

#ifndef STREAMMEDIADEMO_SM_VIDEO_DISPLAY_H
#define STREAMMEDIADEMO_SM_VIDEO_DISPLAY_H

#include <stdio.h>
#include "sm_video_data.h"
#include "../model/sm_video_data.h"

typedef  struct SmVideoDisplay_ *SmVideoDisplay;

//具体实现相关
typedef struct SmVideoDisplayOpaque_ * SmVideoDisplayOpaque;

typedef struct SmVideoDisplay_{
    void (*SetVideoParam)(SmVideoDisplay videoDisplay,SmVideoParam *videoParam);
    SmVideoParam * (*GetVideoParam)(SmVideoDisplay videoDisplay);

    SmVideoDisplayOpaque videoDisplayOpaque;

    int (*VideoDisplayInit)(SmVideoDisplay videoDisplay);
    int (*VideoDisplayRelease)(SmVideoDisplay videoDisplay);

}SmVideoDisplay_;


SmVideoDisplay VideoDisplayCreate();


#endif //STREAMMEDIADEMO_SM_VIDIO_DISPLAY_H
