//
// Created by dengjun on 2018/11/16.
//

#ifndef STREAMMEDIADEMO_SM_VIDEO_DISPLAY_H
#define STREAMMEDIADEMO_SM_VIDEO_DISPLAY_H

#include <stdio.h>

typedef struct VideoDisplay{

}VideoDisplay;


VideoDisplay* video_display_create();
int video_display_init(VideoDisplay *videoDisplay);
void video_display_release(VideoDisplay *videoDisplay);


#endif //STREAMMEDIADEMO_SM_VIDIO_DISPLAY_H
