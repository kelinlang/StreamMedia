//
// Created by dengjun on 2019/3/22.
//

#ifndef VOICELIBDEMO_CLOUDVOICE_ANDROID_VIDEO_DISPLAY_H
#define VOICELIBDEMO_CLOUDVOICE_ANDROID_VIDEO_DISPLAY_H

#include <library/media/cloudvoice_media_callback.h>

#include <library/media/model/cloudvoice_audio_video_model.h>
#include <android/native_window.h>

typedef struct CloudVoiceDisplayOpaque_* CloudVoiceDisplayOpaque;

typedef struct CloudVoiceDisplay_ * CloudVoiceDisplay;

typedef struct CloudVoiceDisplay_{
    CloudVoiceDisplayOpaque displayOpaque;
    void *externHandle;

    void (*setParam)(CloudVoiceDisplay self,CloudVoiceDisplayParam videoParam);
    CloudVoiceDisplayParam (*getParam)(CloudVoiceDisplay display);

    void (*setVideoSurface)(CloudVoiceDisplay self,ANativeWindow *nativeWindow);
    void (*setVideoMatrix)(CloudVoiceDisplay self,float * matrixs);

    void (*setMediaStatusCallback)(CloudVoiceDisplay self, MediaStatusCallback mediaStatusCallback);
    void (*setMediaDataCallback)(CloudVoiceDisplay self, MediaDataCallback mediaDataCallback);

    void (*addData)(CloudVoiceDisplay self, CloudVoiceAVPacket srcPackect);



    void (*start)(CloudVoiceDisplay self);
    void (*stop)(CloudVoiceDisplay self);
    void (*pause)(CloudVoiceDisplay self);
    void (*resume)(CloudVoiceDisplay self);
    void (*onChanged)(CloudVoiceDisplay self);
    void (*destroy)(CloudVoiceDisplay self);

}CloudVoiceDisplay_;


CloudVoiceDisplay cloudVoiceCreateAndroidVideoDisplay(/*CloudVoiceVideoParam displayParam*/);

#endif //VOICELIBDEMO_CLOUDVOICE_ANDROID_VIDEO_DISPLAY_H
