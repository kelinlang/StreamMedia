//
// Created by dengjun on 2019/3/22.
//

#include <library/CommonLib/queue/cloudvoice_blocking_queue.h>
#include <sys/types.h>
#include <library/CommonLib/log/cloudvoice_log.h>
#include <pthread.h>
#include <malloc.h>
#include <memory.h>
#include <library/librtmp/rtmp.h>
#include <library/CommonLib/utils/cloudvoice_utils.h>
#include "cloudvoice_rtmp_client.h"
typedef  struct CloudVoiceRtmpClientOpaque_{
    CloudVoiceRtmpParam rtmpParam;
    int workFlag;
    pthread_t workThreadId;

    CloudVoiceBlockingQueue blockingQueue;

    MediaStatusCallback mediaStatusCallback;
    MediaDataCallback mediaDataCallback;



    RTMP *rtmp;
}CloudVoiceRtmpClientOpaque_;


static void setUrl(CloudVoiceRtmpParam playerParam, char *url){
    if (playerParam && url){
        cloudVoiceStringCopy(url,&playerParam->url);
    }
}

CloudVoiceRtmpParam cloudVoiceCreateRtmpParam(){
    CloudVoiceRtmpParam  rtmpParam = (CloudVoiceRtmpParam)malloc(sizeof(CloudVoiceRtmpParam_));
    if (rtmpParam){
        rtmpParam->setUrl = setUrl;
    }
    return rtmpParam;
}

void  cloudVoiceDestroyRtmpParam(CloudVoiceRtmpParam rtmpParam){
    if (rtmpParam){
        if (rtmpParam->url){
            free(rtmpParam->url);
        }
        free(rtmpParam);
    }
}

void setId(CloudVoiceRtmpClient rtmpClient,char *id){
    if(rtmpClient && rtmpClient->rtmpClientOpaque && id){
        int len = strlen(id)+1;
        if(len > 0){
            rtmpClient->id = (char*)malloc(len);
            stpncpy(rtmpClient->id,id,len);
        }
    }
}

static void setParam(CloudVoiceRtmpClient rtmpClient,CloudVoiceRtmpParam rtmpParam){
    if(rtmpClient && rtmpClient->rtmpClientOpaque && rtmpParam){
        rtmpClient->rtmpClientOpaque->rtmpParam = rtmpParam;
    }
}


static CloudVoiceRtmpParam getParam(CloudVoiceRtmpClient rtmpClient){
    CloudVoiceRtmpParam rtmpParam = NULL;
    if(rtmpClient && rtmpClient->rtmpClientOpaque ){
        rtmpParam = rtmpClient->rtmpClientOpaque->rtmpParam;
    }
    return rtmpParam;
}

static void setMediaStatusCallback(CloudVoiceRtmpClient rtmpClient,MediaStatusCallback mediaStatusCallback){
    if(rtmpClient && rtmpClient->rtmpClientOpaque){
        rtmpClient->rtmpClientOpaque->mediaStatusCallback = mediaStatusCallback;
    }
}

static void setMediaDataCallback(CloudVoiceRtmpClient rtmpClient,MediaDataCallback mediaDataCallback){
    if(rtmpClient && rtmpClient->rtmpClientOpaque){
        rtmpClient->rtmpClientOpaque->mediaDataCallback = mediaDataCallback;
    }
}

static void sendData(CloudVoiceRtmpClient rtmpClient,CloudVoiceAVPacket srcPackect){
    if(rtmpClient && rtmpClient ->rtmpClientOpaque && rtmpClient->rtmpClientOpaque->workFlag == 1){
        rtmpClient->rtmpClientOpaque->blockingQueue->add(rtmpClient->rtmpClientOpaque->blockingQueue,srcPackect);
    } else{
        cloudVoiceDestroyAVPackect(srcPackect);
    }
}


static void sendRtmpPacket(CloudVoiceRtmpClient rtmpClient, unsigned int nPacketType,
                           unsigned char *data, unsigned int size,
                           unsigned int nTimestamp) {
//    LOGI("rtmp push size : %d",size);

    RTMPPacket *packet = (RTMPPacket *) malloc(sizeof(RTMPPacket));
    RTMPPacket_Alloc(packet, size);
    RTMPPacket_Reset(packet);
    if (nPacketType == RTMP_PACKET_TYPE_INFO) { // metadata
        packet->m_nChannel = 0x03;
    } else if (nPacketType == RTMP_PACKET_TYPE_VIDEO) { // video
        packet->m_nChannel = 0x04;
    } else if (nPacketType == RTMP_PACKET_TYPE_AUDIO) { //audio
        packet->m_nChannel = 0x05;
    } else {
        packet->m_nChannel = -1;
    }
    packet->m_nInfoField2 = rtmpClient->rtmpClientOpaque->rtmp->m_stream_id;
    memcpy(packet->m_body, data, size);

    packet->m_headerType = RTMP_PACKET_SIZE_LARGE;
    packet->m_hasAbsTimestamp = FALSE;
    packet->m_nTimeStamp = nTimestamp;
    packet->m_packetType = nPacketType;
    packet->m_nBodySize = size;
    int ret = RTMP_SendPacket(rtmpClient->rtmpClientOpaque->rtmp, packet, 1);

//    LOGD("RTMP_SendPacket ret = %d", ret);
    if (!ret) {
        cloudVoiceLogE("RTMP_SendPacket   error------sockerr :");
        rtmpClient->rtmpClientOpaque->workFlag = 0;
    }

    RTMPPacket_Free(packet);
    free(packet);
}

static void initRtmpClient(CloudVoiceRtmpClient rtmpClient) {
    cloudVoiceLogI("-----------initRtmp  Client------------");

//    RTMP_LogSetCallback(rtmpLogPrint);

    char *url = "rtmp://192.168.3.34:1395/mylive/rtmpstream";
    if (rtmpClient) {
        rtmpClient->rtmpClientOpaque->rtmp = RTMP_Alloc();
        if (rtmpClient->rtmpClientOpaque->rtmp) {
            RTMP_Init(rtmpClient->rtmpClientOpaque->rtmp);
            int ret = RTMP_SetupURL(rtmpClient->rtmpClientOpaque->rtmp, url);
            if (!ret) {
                RTMP_Free(rtmpClient->rtmpClientOpaque->rtmp);
                rtmpClient->rtmpClientOpaque->rtmp = NULL;
                cloudVoiceLogI("push RTMP_SetupURL=%d", ret);
                return;
            }

            if (rtmpClient->rtmpClientOpaque->rtmpParam->clientType == PUSH){
                RTMP_EnableWrite(rtmpClient->rtmpClientOpaque->rtmp);
            }

            ret = RTMP_Connect(rtmpClient->rtmpClientOpaque->rtmp, NULL);
            if (!ret) {
                RTMP_Free(rtmpClient->rtmpClientOpaque->rtmp);
                rtmpClient->rtmpClientOpaque->rtmp = NULL;
                cloudVoiceLogI("push RTMP_Connect=%d", ret);
                return;
            }

            ret = RTMP_ConnectStream(rtmpClient->rtmpClientOpaque->rtmp, 0);
            if (!ret) {
                RTMP_Close(rtmpClient->rtmpClientOpaque->rtmp);
                RTMP_Free(rtmpClient->rtmpClientOpaque->rtmp);
                rtmpClient->rtmpClientOpaque->rtmp = NULL;
                cloudVoiceLogI("push RTMP_ConnectStream=%d", ret);
                return;
            }
            cloudVoiceLogI("push RTMP_ConnectStream=%d, -----------------success---------------", ret);
            cloudVoiceLogI("");
            cloudVoiceLogI("");
            cloudVoiceLogI("");
            cloudVoiceLogI("");
            cloudVoiceLogI("");
            rtmpClient->rtmpClientOpaque->workFlag = 1;
        }
    }
}

static void sendSpsPpsData(CloudVoiceRtmpClient rtmpPushClient, CloudVoiceAVPacket videoData){
    int bodySize = videoData->spsLen + videoData->ppsLen + 16;
    unsigned char *body = (unsigned char *) malloc(bodySize);
    int i = 0;
    //frame type(4bit)和CodecId(4bit)合成一个字节(byte)
    //frame type 关键帧1  非关键帧2
    //CodecId  7表示avc
    body[i++] = 0x17;

    //fixed 4byte
    body[i++] = 0x00;
    body[i++] = 0x00;
    body[i++] = 0x00;
    body[i++] = 0x00;

    //configurationVersion： 版本 1byte
    body[i++] = 0x01;

    //AVCProfileIndication：Profile 1byte  sps[1]
    body[i++] = videoData->sps[1];

    //compatibility：  兼容性 1byte  sps[2]
    body[i++] = videoData->sps[2];

    //AVCLevelIndication： ProfileLevel 1byte  sps[3]
    body[i++] = videoData->sps[3];

    //lengthSizeMinusOne： 包长数据所使用的字节数  1byte
    body[i++] = 0xff;

    //sps个数 1byte
    body[i++] = 0xe1;
    //sps长度 2byte
    body[i++] = (videoData->spsLen >> 8) & 0xff;
    body[i++] = videoData->spsLen & 0xff;

    //sps data 内容
    memcpy(&body[i], videoData->sps, videoData->spsLen);
    i += videoData->spsLen;
    //pps个数 1byte
    body[i++] = 0x01;
    //pps长度 2byte
    body[i++] = (videoData->ppsLen >> 8) & 0xff;
    body[i++] = videoData->ppsLen & 0xff;
    //pps data 内容
    memcpy(&body[i], videoData->pps, videoData->ppsLen);
    sendRtmpPacket(rtmpPushClient, RTMP_PACKET_TYPE_VIDEO, body, bodySize, videoData->timeStamp);
    free(body);
}

static void sendH264Data(CloudVoiceRtmpClient rtmpPushClient, CloudVoiceAVPacket videoData) {
    if (videoData && videoData->dataLen > 11) {
        int size = videoData->dataLen + 9;
        unsigned char *body = (unsigned char *) malloc(size);
        memset(body, 0, size);

        int i = 0;
        body[i++] = videoData->frameType ? 0x17 : 0x27;
        body[i++] = 0x01;// AVC NALU
        body[i++] = 0x00;
        body[i++] = 0x00;
        body[i++] = 0x00;

        // NALU size
        body[i++] = videoData->dataLen >> 24 & 0xff;
        body[i++] = videoData->dataLen >> 16 & 0xff;
        body[i++] = videoData->dataLen >> 8 & 0xff;
        body[i++] = videoData->dataLen & 0xff;
        // NALU data
//        LOGI("pixelsData pos 3 : %d, SmVideoData : %d",videoData->pixelsData,videoData);
        memcpy(&body[i], videoData->data+videoData->startPos, videoData->dataLen);
//        LOGI("pixelsData pos 4 : %d, SmVideoData : %d",videoData->pixelsData,videoData);


        sendRtmpPacket(rtmpPushClient, RTMP_PACKET_TYPE_VIDEO, body, size, videoData->timeStamp);
        free(body);
    }
}

static void pushLoop(CloudVoiceRtmpClient rtmpClient){
    while (rtmpClient->rtmpClientOpaque->workFlag == 1) {
        CloudVoiceAVPacket avPackect = rtmpClient->rtmpClientOpaque->blockingQueue->take(rtmpClient->rtmpClientOpaque->blockingQueue);
        if (avPackect){
            switch (avPackect->packetFormat){
                case VDIEO_FORMAT_H264:
                    sendH264Data(rtmpClient,avPackect);
                    break;
                case VDIEO_FORMAT_H264_SPS_PPS:
                    sendSpsPpsData(rtmpClient,avPackect);
                    break;
            }
            cloudVoiceDestroyAVPackect(avPackect);
        }
    }
}


static void pullLoop(CloudVoiceRtmpClient rtmpClient){
    RTMPPacket *packet = (RTMPPacket *) malloc(sizeof(RTMPPacket));
    while (rtmpClient->rtmpClientOpaque->workFlag == 1) {
        if (RTMP_ReadPacket(rtmpClient->rtmpClientOpaque->rtmp,packet)){
//        LOGI("read rtmp packet type : %d",packet->m_packetType);
            if(RTMPPacket_IsReady(packet)){
                switch (packet->m_packetType){
                    case RTMP_PACKET_TYPE_VIDEO:
//                        LOGI("read rtmp packet size : %d,  mBody : %d",packet->m_nBodySize, packet->m_body);

                        if (rtmpClient->rtmpClientOpaque->mediaDataCallback && packet->m_body[1] == 0x01){
                            CloudVoiceAVPacket avPacket = cloudVoiceCreateAVPackect();
                            int dataLen = packet->m_nBodySize-9;
                            uint8_t *data = (uint8_t*)malloc(packet->m_nBodySize-9);
                            memcpy(data,packet->m_body-9,packet->m_nBodySize+9);
                            avPacket->data = data;
                            avPacket->startPos = 0;
                            avPacket->dataLen = dataLen;
                            avPacket->packetFormat = VDIEO_FORMAT_H264;
                            rtmpClient->rtmpClientOpaque->mediaDataCallback(rtmpClient->externHandle,NULL,0,avPacket,NULL);
                        }
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
    RTMPPacket_Free(packet);
    free(packet);
}


static void releaseRtmpClient(CloudVoiceRtmpClient rtmpPushClient) {
    if (rtmpPushClient && rtmpPushClient->rtmpClientOpaque) {
        if (rtmpPushClient->rtmpClientOpaque->rtmp){
            RTMP_Close(rtmpPushClient->rtmpClientOpaque->rtmp);
            RTMP_Free(rtmpPushClient->rtmpClientOpaque->rtmp);
            rtmpPushClient->rtmpClientOpaque->rtmp = NULL;
        }
        rtmpPushClient->rtmpClientOpaque->workFlag = 0;
    }
}

static void* workThreadFunc(void *arg){
    CloudVoiceRtmpClient rtmpClient = (CloudVoiceRtmpClient)arg;
    initRtmpClient(rtmpClient);

    if (rtmpClient->rtmpClientOpaque->workFlag ==1){
        if (rtmpClient->rtmpClientOpaque->rtmpParam->clientType == PUSH){
            pushLoop(rtmpClient);
        } else{
            pullLoop(rtmpClient);
        }
    }

    releaseRtmpClient(rtmpClient);
    cloudVoiceLogI("rtmpClient thread  finish");
}

static void start(CloudVoiceRtmpClient rtmpClient){
    if (rtmpClient && rtmpClient->rtmpClientOpaque){
        rtmpClient->rtmpClientOpaque->workFlag = 0;
        int retval = pthread_create(&rtmpClient->rtmpClientOpaque->workThreadId,NULL,workThreadFunc,rtmpClient);
//
        cloudVoiceLogI("start rtmp thread retval : %d", retval);
    }
}

static void stop(CloudVoiceRtmpClient rtmpClient){
    if (rtmpClient && rtmpClient->rtmpClientOpaque){
        if(rtmpClient->rtmpClientOpaque->workThreadId != -1){
            cloudVoiceLogI("pthread_join  start");
            pthread_join(rtmpClient->rtmpClientOpaque->workThreadId,NULL);
            cloudVoiceLogI("pthread_join  finish");
            rtmpClient->rtmpClientOpaque->workThreadId = -1;
        }
    }
}

static void destroy(CloudVoiceRtmpClient rtmpClient){
    if (rtmpClient && rtmpClient->rtmpClientOpaque){
        stop(rtmpClient);
        if (rtmpClient->rtmpClientOpaque->blockingQueue){
            rtmpClient->rtmpClientOpaque->blockingQueue->destroy(rtmpClient->rtmpClientOpaque->blockingQueue);
            rtmpClient->rtmpClientOpaque->blockingQueue = NULL;
        }
        /*if (rtmpClient->rtmpClientOpaque->cacheList){
            rtmpClient->rtmpClientOpaque->cacheList->destroy(rtmpClient->rtmpClientOpaque->cacheList);
            rtmpClient->rtmpClientOpaque->cacheList = NULL;
        }*/
        cloudVoiceDestroyRtmpParam(rtmpClient->rtmpClientOpaque->rtmpParam);
        free(rtmpClient->rtmpClientOpaque);
        free(rtmpClient);
    }

}
static void cloudVoiceAVPackactFreeCallback(void *object, void *element){
    cloudVoiceDestroyAVPackect((CloudVoiceAVPacket)element);
}




CloudVoiceRtmpClient cloudVoiceCreateRtmpClient(){
    CloudVoiceRtmpClient rtmpClient = (CloudVoiceRtmpClient)malloc(sizeof(CloudVoiceRtmpClient_));
    if(rtmpClient){
        rtmpClient->setId = setId;
        rtmpClient->getParam = getParam;
        rtmpClient->setParam = setParam;
        rtmpClient->setMediaStatusCallback = setMediaStatusCallback;
        rtmpClient->setMediaDataCallback = setMediaDataCallback;
        rtmpClient->sendData = sendData;
        rtmpClient->start = start;
        rtmpClient->stop = stop;
        rtmpClient->destroy = destroy;
        rtmpClient->externHandle = NULL;

        CloudVoiceRtmpClientOpaque rtmpClientOpaque = (CloudVoiceRtmpClientOpaque)malloc(sizeof(CloudVoiceRtmpClientOpaque_));
        if (rtmpClientOpaque){
            rtmpClient->rtmpClientOpaque = rtmpClientOpaque;

            rtmpClientOpaque->workThreadId = -1;
            rtmpClientOpaque->workFlag = 0;
            rtmpClientOpaque->rtmp = NULL;
            rtmpClientOpaque->mediaStatusCallback = NULL;
            rtmpClientOpaque->mediaDataCallback = NULL;
            rtmpClientOpaque->blockingQueue = cloudVoiceCreateBlockingQueue(cloudVoiceAVPackactFreeCallback);
//            rtmpClientOpaque->cacheList = cloudVoiceCreateList(cloudVoiceAVPackactFreeCallback);

            rtmpClientOpaque->rtmpParam = cloudVoiceCreateRtmpParam();
        }
    }
    return rtmpClient;
}