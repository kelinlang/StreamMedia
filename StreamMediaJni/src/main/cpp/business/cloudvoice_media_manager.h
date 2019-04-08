//
// Created by dengjun on 2019/3/26.
//

#ifndef VOICELIBDEMO_CLOUDVOICE_MEDIA_MANAGER_H
#define VOICELIBDEMO_CLOUDVOICE_MEDIA_MANAGER_H

#include <library/CommonLib/map/map.h>
#include <business/model/cloudvoice_stream_param.h>
#include <library/media/model/cloudvoice_audio_video_model.h>
#include <business/model/cloudvoice_media_params.h>
#include <android/native_window.h>



typedef struct CloudVoiceMediaManager_* CloudVoiceMediaManager;

typedef struct CloudVoiceMediaManager_{
    int initFlag;
    map_void_t pullStreamPlayerMap;
    map_void_t pushStreamClientMap;



    void (*init)(CloudVoiceMediaManager mediaManager);
    void (*release)(CloudVoiceMediaManager mediaManager);

    void (*setMediaStatusCallback)(CloudVoiceMediaManager mediaManager);
    void (*setParam)(CloudVoiceMediaManager mediaManager,CloudVoiceMediaParam mediaParam);

    void (*resume)(CloudVoiceMediaManager mediaManager);
    void (*pause)(CloudVoiceMediaManager mediaManager);


    void (*createPlayer)(CloudVoiceMediaManager mediaManager, char*id);
    void (*setVideoSurface)(CloudVoiceMediaManager mediaManager,char *id, ANativeWindow * nativeWindow);
    void (*setVideoMatrix)(CloudVoiceMediaManager mediaManager,char *id,float* matrix );

    void (*setPlayerParam)(CloudVoiceMediaManager mediaManager,char*id,CloudVoicePlayerParam playerParam);
    CloudVoicePlayerParam (*getPlayerParam)(CloudVoiceMediaManager mediaManager,char*id);
    void (*startPlay)(CloudVoiceMediaManager mediaManager,char*id);
    void (*stopPlay)(CloudVoiceMediaManager mediaManager,char*id);

    void (*createPushClient)(CloudVoiceMediaManager mediaManager, char*id);
    void (*setPushStreamParam)(CloudVoiceMediaManager mediaManager,char*id,CloudVoiceStreamParam streamParam);
    void (*startPush)(CloudVoiceMediaManager mediaManager,char*id);
    void (*stopPush)(CloudVoiceMediaManager mediaManager,char*id);

    void (*sendVideoData)(CloudVoiceMediaManager mediaManager,char*id,CloudVoiceAVPacket avPacket);
    void (*sendAudioData)(CloudVoiceMediaManager mediaManager,char*id,CloudVoiceAVPacket avPacket);


}CloudVoiceMediaManager_;


CloudVoiceMediaManager cloudVoiceCreateMediaManager();



#endif //VOICELIBDEMO_CLOUDVOICE_MEDIA_MANAGER_H
