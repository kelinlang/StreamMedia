//
// Created by dengjun on 2019/3/18.
//

#include <librtmp/rtmp.h>
#include <sys/types.h>
#include <malloc.h>
#include <CommonLib/log/cl_sdl_log.h>
#include <pthread.h>
#include <memory.h>
#include "RtmpPullClient.h"


struct SmRtmpPullClient_ {
    RTMP *rtmp;
    uint8_t openFlag;

    pthread_t sendThreadId;


    DataCallback  dataCallback;
} SmRtmpPullClient_;


static void initRtmpClient(SmRtmpPullClient rtmpPullClient) {
    LOGI("-----------init Rtmp pull Client------------");

//    RTMP_LogSetCallback(rtmpLogPrint);

    char *url = "rtmp://192.168.3.209:1395/mylive/rtmpstream";
    if (rtmpPullClient) {
        rtmpPullClient->rtmp = RTMP_Alloc();
        if (rtmpPullClient->rtmp) {
            RTMP_Init(rtmpPullClient->rtmp);
            int ret = RTMP_SetupURL(rtmpPullClient->rtmp, url);
            if (!ret) {
                RTMP_Free(rtmpPullClient->rtmp);
                rtmpPullClient->rtmp = NULL;
                LOGD("RTMP_SetupURL=%d", ret);
                return;
            }
//            RTMP_EnableWrite(rtmpPullClient->rtmp);

            ret = RTMP_Connect(rtmpPullClient->rtmp, NULL);
            if (!ret) {
                RTMP_Free(rtmpPullClient->rtmp);
                rtmpPullClient->rtmp = NULL;
                LOGD("RTMP_Connect=%d", ret);
                return;
            }

            ret = RTMP_ConnectStream(rtmpPullClient->rtmp, 0);
            if (!ret) {
                RTMP_Close(rtmpPullClient->rtmp);
                RTMP_Free(rtmpPullClient->rtmp);
                rtmpPullClient->rtmp = NULL;
                LOGD("rtmp pull client  RTMP_ConnectStream=%d", ret);
                return;
            }
            LOGI("rtmp pull client  RTMP_ConnectStream=%d, -----------------success---------------", ret);
            LOGI("");
            LOGI("");
            LOGI("");
            LOGI("");
            LOGI("");
            rtmpPullClient->openFlag = 1;
        }
    }
}

static void releaseRtmpClient(SmRtmpPullClient rtmpPushClient) {
    if (rtmpPushClient && rtmpPushClient->rtmp) {
        RTMP_Close(rtmpPushClient->rtmp);
        RTMP_Free(rtmpPushClient->rtmp);
        rtmpPushClient->rtmp = NULL;
        rtmpPushClient->openFlag = 0;
    }
}

SmRtmpPullClient smCreateRtmpPullClient(){
    SmRtmpPullClient rtmpPullClient = (SmRtmpPullClient) malloc(sizeof(struct SmRtmpPullClient_));
    if (rtmpPullClient) {
        rtmpPullClient->openFlag = 0;
        rtmpPullClient->rtmp = NULL;
    }
    return rtmpPullClient;
}

void smDestoryRtmpPullClient(SmRtmpPullClient rtmpPullClient){

}

static void *readThreadFunc(void *arg) {
    LOGI("rtmp pull client readThreadFunc start");
    SmRtmpPullClient rtmpPullClient = (SmRtmpPullClient) arg;

    initRtmpClient(rtmpPullClient);

    char *buffer = (char*)malloc(640*480);
    RTMPPacket *packet = (RTMPPacket *) malloc(sizeof(RTMPPacket));
    while (rtmpPullClient->openFlag == 1) {

        if (RTMP_ReadPacket(rtmpPullClient->rtmp,packet)){
//        LOGI("read rtmp packet type : %d",packet->m_packetType);
            if(RTMPPacket_IsReady(packet)){
                switch (packet->m_packetType){
                    case RTMP_PACKET_TYPE_VIDEO:
                        LOGI("read rtmp packet size : %d,  mBody : %d",packet->m_nBodySize, packet->m_body);
                        char *h264Data = (char*)malloc(packet->m_nBodySize-9);
                        memcpy(h264Data, packet->m_body+9, packet->m_nBodySize-9);
                        if (rtmpPullClient->dataCallback && packet->m_body[1] == 0x01){
                            rtmpPullClient->dataCallback(0,packet->m_nBodySize,h264Data,NULL);
                        }
                        free(h264Data);
                        break;
                    case RTMP_PACKET_TYPE_INFO:
                        break;
                }
            }

            RTMPPacket_Free(packet);

        } else{

            break;
        }

    }
    free(packet);

    free(buffer);
    releaseRtmpClient(rtmpPullClient);


    LOGI("rtmp pull client  readThreadFunc finish");
}

void smRtmpPullClientStart(SmRtmpPullClient rtmpPullClient){
    if (rtmpPullClient) {
        int retval1 = pthread_create(&rtmpPullClient->sendThreadId, NULL, readThreadFunc,
                                     rtmpPullClient);
        LOGD("smRtmpPullClientStart retval : %d,  send threat id : %d", retval1,rtmpPullClient->sendThreadId);
    }
}

void smRtmpPullClientStop(SmRtmpPullClient rtmpPullClient){
    if (rtmpPullClient) {
        rtmpPullClient->openFlag = 0;
        if (rtmpPullClient->sendThreadId != -1) {
            LOGD("pthread_join  start 2");
            pthread_join(rtmpPullClient->sendThreadId, NULL);
            LOGD("pthread_join  finish 2");
            rtmpPullClient->sendThreadId = -1;
        }
    }
}

void smRtmpPullClientSetDataCallback(SmRtmpPullClient rtmpPullClient, DataCallback dataCallback){
    if(rtmpPullClient){
        rtmpPullClient->dataCallback = dataCallback;
    }
}