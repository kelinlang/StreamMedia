//
// Created by dengjun on 2018/12/25.
//

#ifndef STREAMMEDIADEMO_SM_VIDEO_DATA_BUFFER_H
#define STREAMMEDIADEMO_SM_VIDEO_DATA_BUFFER_H

#include "../model/sm_video_data.h"
#include "../../CommonLib/thread/cl_sdl_mutex.h"

#define MAX_CACHE_FRAME  30

typedef struct SmVideoDataNode_* SmVideoDataNode;
typedef struct SmVideoDataNode_{
    SmVideoData videoData;
    int serial;
    SmVideoDataNode next;
}SmVideoDataNode_;


//多线程，需要多线程同步
typedef struct SmVideoDataQueue_* SmVideoDataQueue;
typedef struct SmVideoDataQueue_{
    SmVideoDataNode firstNode;
    SmVideoDataNode lastNode;
    int numNode;
    ClMutex clMutex;
    ClCond clCond;
    int abortFlag;

    SmVideoDataNode recycleNode;
    int numRecycleNode;
    ClMutex recycleClMutex;

}  SmVideoDataQueue_;


SmVideoDataNode smCreateVideoDataNode();
void smDestoryVideoDataNode(SmVideoDataNode videoDataNode);


SmVideoDataQueue smCreateVideoDataQueue();
void smDestroyVideoDataQueue(SmVideoDataQueue videoDataQueue);
void smVideoDataQueueClearData(SmVideoDataQueue videoDataQueue);
void smVideoDataQueueAbort(SmVideoDataQueue videoDataQueue);
int smGetVideoDataQueueSize(SmVideoDataQueue videoDataQueue);
void smVideoDataQueueEnqueueData(SmVideoDataQueue videoDataQueue, SmVideoDataNode videoDataNode);
SmVideoDataNode  smVideoDataQueueDequeueData(SmVideoDataQueue videoDataQueue);

SmVideoDataNode smCreateVideoDataNodeFromCache(SmVideoDataQueue videoDataQueue);
void smCacheVideoDataNodeToCache(SmVideoDataQueue videoDataQueue,SmVideoDataNode videoDataNode);

#endif //STREAMMEDIADEMO_SM_VIDEO_DATA_BUFFER_H
