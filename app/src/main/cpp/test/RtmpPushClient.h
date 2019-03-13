//
// Created by dengjun on 2019/3/12.
//

#ifndef STREAMMEDIADEMO_RTMPPUSHCLIENT_H
#define STREAMMEDIADEMO_RTMPPUSHCLIENT_H

#include <stdint.h>

typedef struct SmRtmpPushClient_* SmRtmpPushClient;

SmRtmpPushClient smCreateRtmpPushClient();

void smRtmpPushClientStart(SmRtmpPushClient rtmpPushClient);
void smRtmpPushClientStop(SmRtmpPushClient rtmpPushClient);
SmVideoDataNode smRtmpPushClientGetCacheVideoData(SmRtmpPushClient rtmpPushClient);
void smRtmpPushClientAddData(SmRtmpPushClient rtmpPushClient, SmVideoDataNode videoDataNode);

#endif //STREAMMEDIADEMO_RTMPPUSHCLIENT_H
