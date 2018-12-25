//
// Created by dengjun on 2018/12/21.
//

#include <malloc.h>
#include "sm_data_buffer.h"
SmVideoDataNode createSmVideoDataNode(){
    SmVideoDataNode videoDataNode = (SmVideoDataNode)malloc(sizeof(struct  SmVideoDataNode_));
    if (videoDataNode != NULL){
        videoDataNode->videoData = NULL;
        videoDataNode->serial = 0;
        videoDataNode->next = NULL;
    }
    return videoDataNode;
}
void SmVideoDataNodeDestroy(SmVideoDataNode videoDataNode){
    if (videoDataNode != NULL){
        if (videoDataNode->videoData != NULL){
            free(videoDataNode->videoData);
        }
        free(videoDataNode);
    }
}
void SmVideoDataNodeClear(SmVideoDataNode videoDataNode){

}
void SmVideoDataNodeAdd(SmVideoDataNode lastNode, SmVideoDataNode newNode){
    if (lastNode && newNode){
        lastNode ->next = newNode;
    }
}

SmVideoDataNode SmVideoDataNodeRemove(SmVideoDataNode fistNode){
    if (fistNode && fistNode->next){
        SmVideoDataNode next = fistNode->next;
        return next;
    } else{
        return NULL;
    }
}

SmVideoDataQueue smCreateVideoDataQueue(){
    SmVideoDataQueue  videoDataQueue = (SmVideoDataQueue)malloc(sizeof(SmVideoDataQueue_));
    if (videoDataQueue){
        videoDataQueue->fistNode = NULL;
        videoDataQueue->lastNode = NULL;
        videoDataQueue->numNode = 0;

        videoDataQueue->recycleNode = NULL;
        videoDataQueue->numRecycleNode = 0;
    }
    return videoDataQueue;
}
void SmVideoDataQueueDestroy(SmVideoDataQueue videoDataQueue){
    if (videoDataQueue){
        SmVideoDataNodeClear(videoDataQueue);
        free(videoDataQueue);
    }
}
void SmVideoDataQueueClear(SmVideoDataQueue videoDataQueue){
    if (videoDataQueue && videoDataQueue->fistNode){
        SmVideoDataNode next = videoDataQueue->fistNode->next;
        SmVideoDataNodeDestroy(videoDataQueue->fistNode);
        while (next){
            next = next->next;
            SmVideoDataNodeDestroy(next);
        }
        videoDataQueue->fistNode = NULL;
        videoDataQueue->lastNode = NULL;
    }
}
void SmVideoDataQueueAdd(SmVideoDataQueue videoDataQueue, SmVideoData videoData){
    if(videoDataQueue && videoData){
        SmVideoDataNode  dataNode = createSmVideoDataNode();
        if(dataNode){
            dataNode->videoData = videoData;
            if (!videoDataQueue->fistNode){
                videoDataQueue->fistNode = dataNode;
            }
            videoDataQueue->lastNode = dataNode;
        }
    }
}
SmVideoData SmVideoDataQueueGet(SmVideoDataQueue videoDataQueue){
    SmVideoData retVideoData = NULL;
    if(videoDataQueue && videoDataQueue->fistNode){
        retVideoData = videoDataQueue->fistNode->videoData;
        SmVideoDataNode tmp = videoDataQueue->fistNode;
        SmVideoDataNodeRemove(videoDataQueue->fistNode);
    }
    return retVideoData;
}
void SmVideoDataQueueCacheData(SmVideoDataQueue videoDataQueue, SmVideoData videoData){

}
SmVideoData SmVideoDataQueueGetCacheData(SmVideoDataQueue videoDataQueue){

}