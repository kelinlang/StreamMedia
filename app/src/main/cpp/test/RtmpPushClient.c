//
// Created by dengjun on 2019/3/12.
//

#include <librtmp/rtmp.h>
#include <malloc.h>
#include <CommonLib/log/cl_sdl_log.h>
#include <librtmp/rtmp-helpers.h>
#include <StreamMedia/model/sm_video_data.h>
#include <StreamMedia/buffer/sm_video_data_buffer.h>
#include "RtmpPushClient.h"
#include <stdarg.h>
#include <librtmp/log.h>

struct SmRtmpPushClient_ {
    RTMP *rtmp;
    uint8_t openFlag;

    pthread_t sendThreadId;
    SmVideoDataQueue videoDataQueue;
} SmRtmpPushClient_;

static void sendRtmpPacket(SmRtmpPushClient rtmpPushClient, unsigned int nPacketType,
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
    packet->m_nInfoField2 = rtmpPushClient->rtmp->m_stream_id;
    memcpy(packet->m_body, data, size);

    packet->m_headerType = RTMP_PACKET_SIZE_LARGE;
    packet->m_hasAbsTimestamp = FALSE;
    packet->m_nTimeStamp = nTimestamp;
    packet->m_packetType = nPacketType;
    packet->m_nBodySize = size;
    int ret = RTMP_SendPacket(rtmpPushClient->rtmp, packet, 1);

//    LOGD("RTMP_SendPacket ret = %d", ret);
    if (!ret) {
        LOGD("RTMP_SendPacket   error------sockerr :");
        rtmpPushClient->openFlag = 0;
    }

    RTMPPacket_Free(packet);
    free(packet);
}
static void sendSpsPpsData(SmRtmpPushClient rtmpPushClient, SmVideoData videoData){
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

static void sendData(SmRtmpPushClient rtmpPushClient, SmVideoData videoData) {
    if (videoData && videoData->pixelsDataLen > 11) {
        int size = videoData->pixelsDataLen + 9;
        unsigned char *body = (unsigned char *) malloc(size);
        memset(body, 0, size);

        int i = 0;
        body[i++] = videoData->frameType ? 0x17 : 0x27;
        body[i++] = 0x01;// AVC NALU
        body[i++] = 0x00;
        body[i++] = 0x00;
        body[i++] = 0x00;

        // NALU size
        body[i++] = videoData->pixelsDataLen >> 24 & 0xff;
        body[i++] = videoData->pixelsDataLen >> 16 & 0xff;
        body[i++] = videoData->pixelsDataLen >> 8 & 0xff;
        body[i++] = videoData->pixelsDataLen & 0xff;
        // NALU data
//        LOGI("pixelsData pos 3 : %d, SmVideoData : %d",videoData->pixelsData,videoData);
        memcpy(&body[i], videoData->pixelsData, videoData->pixelsDataLen);
//        LOGI("pixelsData pos 4 : %d, SmVideoData : %d",videoData->pixelsData,videoData);


        sendRtmpPacket(rtmpPushClient, RTMP_PACKET_TYPE_VIDEO, body, size, videoData->timeStamp);
        free(body);
    }
}

static void rtmpLogPrint(int level, const char *fmt, va_list args) {
    char log[2048];
    vsprintf(log, fmt, args);
    LOGI("%s",log);
}

static void initRtmpClient(SmRtmpPushClient rtmpPushClient) {
    LOGI("-----------initRtmp push Client------------");

//    RTMP_LogSetCallback(rtmpLogPrint);

    char *url = "rtmp://192.168.3.34:1395/mylive/rtmpstream";
    if (rtmpPushClient) {
        rtmpPushClient->rtmp = RTMP_Alloc();
        if (rtmpPushClient->rtmp) {
            RTMP_Init(rtmpPushClient->rtmp);
            int ret = RTMP_SetupURL(rtmpPushClient->rtmp, url);
            if (!ret) {
                RTMP_Free(rtmpPushClient->rtmp);
                rtmpPushClient->rtmp = NULL;
                LOGI("push RTMP_SetupURL=%d", ret);
                return;
            }
            RTMP_EnableWrite(rtmpPushClient->rtmp);

            ret = RTMP_Connect(rtmpPushClient->rtmp, NULL);
            if (!ret) {
                RTMP_Free(rtmpPushClient->rtmp);
                rtmpPushClient->rtmp = NULL;
                LOGI("push RTMP_Connect=%d", ret);
                return;
            }

            ret = RTMP_ConnectStream(rtmpPushClient->rtmp, 0);
            if (!ret) {
                RTMP_Close(rtmpPushClient->rtmp);
                RTMP_Free(rtmpPushClient->rtmp);
                rtmpPushClient->rtmp = NULL;
                LOGI("push RTMP_ConnectStream=%d", ret);
                return;
            }
            LOGI("push RTMP_ConnectStream=%d, -----------------success---------------", ret);
            LOGI("");
            LOGI("");
            LOGI("");
            LOGI("");
            LOGI("");
            rtmpPushClient->openFlag = 1;
        }
    }
}

static void releaseRtmpClient(SmRtmpPushClient rtmpPushClient) {
    if (rtmpPushClient && rtmpPushClient->rtmp) {
        RTMP_Close(rtmpPushClient->rtmp);
        RTMP_Free(rtmpPushClient->rtmp);
        rtmpPushClient->rtmp = NULL;
        rtmpPushClient->openFlag = 0;
    }
}

static void sendMetaData(SmRtmpPushClient rtmpPushClient) {
    if (rtmpPushClient && rtmpPushClient->rtmp && rtmpPushClient->openFlag == 1) {
        int size;
        char buf[4096];
        char *enc = buf;
        char *end = enc + sizeof(buf);
        AMF_EncodeString(enc, end, "onMetaData");

        *enc++ = AMF_ECMA_ARRAY;
        AMF_EncodeInt32(enc, end, 14);

        AMF_EncodeNamedNumber(enc, end, "duration", 0.0);
        AMF_EncodeNamedNumber(enc, end, "fileSize", 0.0);
        AMF_EncodeNamedNumber(enc, end, "width", 640);
        AMF_EncodeNamedNumber(enc, end, "height", 480);
        enc_str_val(&enc, end, "videocodecid", "avc1");
        AMF_EncodeNamedNumber(enc, end, "videodatarate", 640 * 480 * 30);
        AMF_EncodeNamedNumber(enc, end, "framerate", 30);

        AMF_EncodeNamedNumber(enc, end, "duration", 0.0);

        enc_str_val(&enc, end, "audiocodecid",
                    "mp4a"); ////enc_num_val(&enc, end, "audiocodecid",10.0); //aac
        enc_num_val(&enc, end, "audiodatarate", 32000);
        enc_num_val(&enc, end, "audiosamplerate", 44100);
        enc_num_val(&enc, end, "audiosamplesize", 16.0);
        enc_num_val(&enc, end, "audiochannels", (double) 1);

        enc_bool_val(&enc, end, "stereo", 0);
        *enc++ = 0;
        *enc++ = 0;
        *enc++ = AMF_OBJECT_END;
        size = enc - buf;

        LOGD("-----------sendMetaData--------");
        sendRtmpPacket(rtmpPushClient, RTMP_PACKET_TYPE_INFO, buf, size, 0);
    }
}

SmRtmpPushClient smCreateRtmpPushClient() {
    SmRtmpPushClient rtmpPushClient = (SmRtmpPushClient) malloc(sizeof(struct SmRtmpPushClient_));
    if (rtmpPushClient) {
        rtmpPushClient->openFlag = 0;
        rtmpPushClient->rtmp = NULL;
        rtmpPushClient->videoDataQueue = smCreateVideoDataQueue();
    }
    return rtmpPushClient;
}


static void *sendThreadFunc(void *arg) {
    LOGI("sendThreadFunc start");
    SmRtmpPushClient rtmpPushClient = (SmRtmpPushClient) arg;

    initRtmpClient(rtmpPushClient);

    if (rtmpPushClient->openFlag == 1) {
//        sendMetaData(rtmpPushClient);
    }

    while (rtmpPushClient->openFlag == 1) {

        LOGD("send rtmp data first : %d",rtmpPushClient->videoDataQueue);
        SmVideoDataNode videoDataNode = smVideoDataQueueDequeueData(rtmpPushClient->videoDataQueue);

        if (videoDataNode) {
//            LOGD("send rtmp data ");
//            LOGD("  ");
            LOGD("send rtmp data frameType : %d",videoDataNode->videoData->frameType);
            if (videoDataNode->videoData->dataFormat == VDIEO_FORMAT_H264){
                sendData(rtmpPushClient, videoDataNode->videoData);

                if(videoDataNode->videoData->pixelsData){
                    free(videoDataNode->videoData->pixelsData);
                    videoDataNode->videoData->pixelsData = NULL;
                }
            } else if(videoDataNode->videoData->dataFormat == VDIEO_FORMAT_H264_SPS_PPS){
                sendSpsPpsData(rtmpPushClient, videoDataNode->videoData);

                if(videoDataNode->videoData->sps){
                    free(videoDataNode->videoData->sps);
                }
                videoDataNode->videoData->sps = NULL;

                if (videoDataNode->videoData->pps){
                    free(videoDataNode->videoData->pps);
                }
                videoDataNode->videoData->pps = NULL;
            }
            LOGD("send rtmp data end");
        }

        smCacheVideoDataNodeToCache(rtmpPushClient->videoDataQueue, videoDataNode);
    }
    releaseRtmpClient(rtmpPushClient);
    LOGI("sendThreadFunc finish");
}

void smRtmpPushClientStart(SmRtmpPushClient rtmpPushClient) {
    if (rtmpPushClient) {
        if (!rtmpPushClient->videoDataQueue) {
            rtmpPushClient->videoDataQueue = smCreateVideoDataQueue();
        }
        int retval1 = pthread_create(&rtmpPushClient->sendThreadId, NULL, sendThreadFunc,
                                     rtmpPushClient);
        LOGD("smRtmpPushClientStart retval : %d,  send threat id : %d", retval1,rtmpPushClient->sendThreadId);
    }
}


void smRtmpPushClientStop(SmRtmpPushClient rtmpPushClient) {
    if (rtmpPushClient) {
        rtmpPushClient->openFlag = 0;
        if (rtmpPushClient->sendThreadId != -1) {
            LOGD("pthread_join  start 2");
            pthread_join(rtmpPushClient->sendThreadId, NULL);
            LOGD("pthread_join  finish 2");
            rtmpPushClient->sendThreadId = -1;
        }
        if (rtmpPushClient->videoDataQueue) {
            smDestroyVideoDataQueue(rtmpPushClient->videoDataQueue);
            rtmpPushClient->videoDataQueue = NULL;
        }
    }
}


SmVideoDataNode smRtmpPushClientGetCacheVideoData(SmRtmpPushClient rtmpPushClient) {
    SmVideoDataNode videoDataNode = NULL;
    if (rtmpPushClient) {
        LOGD("%s   SmRtmpPushClient : %d",__FUNCTION__,rtmpPushClient);
        videoDataNode = smCreateVideoDataNodeFromCache(rtmpPushClient->videoDataQueue);

      /*  if (videoDataNode && videoDataNode->videoData){
            SmVideoData videoData = videoDataNode->videoData;

            videoData->pixelsDataLen = videoData->pixelsDataLen;
            if (!videoData->pixelsData) {
//                videoData->pixelsData = (uint8_t *) malloc(640 * 480);//后面再考虑重用内存
            }
        }*/
    }
    return videoDataNode;
}

void smRtmpPushClientAddData(SmRtmpPushClient rtmpPushClient, SmVideoDataNode videoDataNode) {
    if (rtmpPushClient && videoDataNode != NULL
            && rtmpPushClient->openFlag == 1) {
//        LOGI("input data bufidx h264DataLen : %d",h264DataLen);


        /*  SmVideoDataNode videoDataNode = smCreateVideoDataNodeFromCache(rtmpPushClient->videoDataQueue);

          SmVideoData videoData = videoDataNode->videoData;

          videoData->pixelsDataLen = videoData->pixelsDataLen;
          if(!videoData->pixelsData){
              LOGI("smMediaCodecAddData  malloc");
              videoData->pixelsData = (uint8_t *) malloc(640*480);
          }

          memcpy(videoData->pixelsData, videoData->pixelsData, videoData->pixelsDataLen);*/
        smVideoDataQueueEnqueueData(rtmpPushClient->videoDataQueue, videoDataNode);
    }
}
