//
// Created by dengjun on 2019/3/26.
//

#ifndef VOICELIBDEMO_CLOUDVOICE_MEDIA_MANAGER_H
#define VOICELIBDEMO_CLOUDVOICE_MEDIA_MANAGER_H

extern "C" {
#include <library/CommonLib/map/map.h>
#include <business/model/cloudvoice_stream_param.h>
#include <library/media/model/cloudvoice_audio_video_model.h>
#include <business/model/cloudvoice_media_params.h>

#include <business/client/cloudvoice_push_stream_client.h>
#include <business/client/cloudvoice_pull_stream_player.h>
}

#include <android/native_window.h>

#include <iostream>
#include <map>
#include <string>


using namespace std;

class CloudVoiceMediaManager {

public:
    CloudVoiceMediaManager();

    virtual ~CloudVoiceMediaManager();

private:
    int initFlag;
    map<string, CloudVoicePullStreamPlayer> pullStreamPlayerMap;
    map<string, CloudVoicePushStreamClient> pushStreamClientMap;


public:
    void init();
    void release();
    void setMediaStatusCallback();
    void setParam(CloudVoiceMediaParam mediaParam);

    void resume();
    void pause();

    void createPlayer(char*id);
    void setVideoSurface(char *id, ANativeWindow * nativeWindow);
    void setVideoMatrix(char *id,float* matrix );
    void setPlayerParam(char*id,CloudVoicePlayerParam playerParam);
    CloudVoicePlayerParam getPlayerParam(char*id);
    void startPlay(char*id);
    void stopPlay(char*id);

    void createPushClient(char*id);
    void setPushStreamParam(char*id,CloudVoiceStreamParam streamParam);
    void startPush(char*id);
    void stopPush(char*id);

    void sendVideoData(char*id,CloudVoiceAVPacket avPacket);
    void sendAudioData(char*id,CloudVoiceAVPacket avPacket);


};

#endif //VOICELIBDEMO_CLOUDVOICE_MEDIA_MANAGER_H
