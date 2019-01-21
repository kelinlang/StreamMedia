//
// Created by dengjun on 2018/11/16.
//

#ifndef STREAMMEDIADEMO_SM_VIDEO_DATA_H
#define STREAMMEDIADEMO_SM_VIDEO_DATA_H

#include <stdint.h>

#define SM_GLES2_MAX_PLANE 3


typedef struct SmVideoData_ * SmVideoData;

typedef struct SmVideoData_{
    //格式，入h264,yuv,rgb
    unsigned int dataFormat;
    int width;
    int height;

    int pitches[SM_GLES2_MAX_PLANE]; /**< in bytes, Read-only */  //像素长度
    uint8_t **pixels;//像素数据

    unsigned char* pixelsY;
    unsigned char* pixelsU;
    unsigned char* pixelsV;

    int pixelsDataLen;
    uint8_t * pixelsData;

//    void (*destroy)(SmVideoData videoData);

} SmVideoData_;

SmVideoData smCreateVideoData();
void smDestroyVideoData(SmVideoData videoData);


typedef struct SmVideoParam_ * SmVideoParam;

typedef struct SmVideoParam_{
    int viewWidth;
    int viewHeight;
    int gravity;

    unsigned int dataFormat;
    unsigned int displayFormat;

//    void (*destroy)(SmVideoParam videoParam);

    float*matrix;
}SmVideoParam_;

SmVideoParam smCreateVideoParam();
void smDestroyVideoParam(SmVideoParam videoParam);

#endif //STREAMMEDIADEMO_SM_VIDEO_DATA_H
