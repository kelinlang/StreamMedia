//
// Created by dengjun on 2018/12/21.
//

#ifndef STREAMMEDIADEMO_SM_DATA_BUFFER_H
#define STREAMMEDIADEMO_SM_DATA_BUFFER_H


#include "../model/sm_video_data.h"

typedef struct SmVideoDataNode_* SmVideoDataNode;
typedef struct SmVideoDataNode_{
    SmVideoData videoData;
    int serial;
    SmVideoDataNode next;
}SmVideoDataNode_;

SmVideoDataNode createSmVideoDataNode();
void SmVideoDataNodeDestroy(SmVideoDataNode videoDataNode);
void SmVideoDataNodeClear(SmVideoDataNode videoDataNode);
void SmVideoDataNodeAdd(SmVideoDataNode lastNode, SmVideoDataNode newNode);
SmVideoDataNode SmVideoDataNodeRemove(SmVideoDataNode fistNode);

//多线程，需要多线程同步
typedef struct SmVideoDataQueue_* SmVideoDataQueue;
typedef struct SmVideoDataQueue_{
    SmVideoDataNode fistNode;
    SmVideoDataNode lastNode;
    int numNode;

    SmVideoDataNode recycleNode;
    int numRecycleNode;

}  SmVideoDataQueue_;

SmVideoDataQueue smCreateVideoDataQueue();
void SmVideoDataQueueDestroy(SmVideoDataQueue videoDataQueue);
void SmVideoDataQueueClear(SmVideoDataQueue videoDataQueue);
void SmVideoDataQueueAdd(SmVideoDataQueue videoDataQueue, SmVideoData videoData);
SmVideoData SmVideoDataQueueGet(SmVideoDataQueue videoDataQueue);
void SmVideoDataQueueCacheData(SmVideoDataQueue videoDataQueue, SmVideoData videoData);
SmVideoData SmVideoDataQueueGetCacheData(SmVideoDataQueue videoDataQueue);




#endif //STREAMMEDIADEMO_SM_DATA_BUFFER_H
