//
// Created by dengjun on 2019/3/18.
//

#ifndef STREAMMEDIADEMO_RTMPPULLCLIENT_H
#define STREAMMEDIADEMO_RTMPPULLCLIENT_H


#include <StreamMedia/model/sm_data_callback_interface.h>

typedef struct SmRtmpPullClient_* SmRtmpPullClient;

SmRtmpPullClient smCreateRtmpPullClient();
void smDestoryRtmpPullClient(SmRtmpPullClient rtmpPullClient);
void smRtmpPullClientStart(SmRtmpPullClient rtmpPullClient);
void smRtmpPullClientStop(SmRtmpPullClient rtmpPullClient);
void smRtmpPullClientSetDataCallback(SmRtmpPullClient rtmpPullClient, DataCallback dataCallback);

#endif //STREAMMEDIADEMO_RTMPPULLCLIENT_H




