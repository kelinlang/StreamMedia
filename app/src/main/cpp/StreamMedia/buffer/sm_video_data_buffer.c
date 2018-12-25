//
// Created by dengjun on 2018/12/25.
//

#include <malloc.h>
#include "sm_video_data_buffer.h"



static SmVideoDataNode smCreateVideoDataNode(){
    SmVideoDataNode videoDataNode = (SmVideoDataNode)malloc(sizeof(struct  SmVideoDataNode_));
    if (videoDataNode != NULL){
        videoDataNode->videoData = NULL;
        videoDataNode->serial = 0;
        videoDataNode->next = NULL;
    }
    return videoDataNode;
}
static void smDestroyVideoDataNode(SmVideoDataNode videoDataNode){
    if (videoDataNode != NULL){
        smDestroyVideoData(videoDataNode->videoData);
        free(videoDataNode);
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
void smDestroyVideoDataQueue(SmVideoDataQueue videoDataQueue){
    if (videoDataQueue){
        smVideoDataQueueClearData(videoDataQueue);
        free(videoDataQueue);
    }
}
void smVideoDataQueueClearData(SmVideoDataQueue videoDataQueue){
    if (videoDataQueue){
        while (videoDataQueue->fistNode){
            SmVideoDataNode tmp = videoDataQueue->fistNode;
            videoDataQueue->fistNode = tmp->next;
            smDestroyVideoDataNode(tmp);
        }

        videoDataQueue->fistNode = NULL;
        videoDataQueue->lastNode = NULL;
        videoDataQueue->numNode = 0;


        if(videoDataQueue->recycleNode){
            while (videoDataQueue->recycleNode){
                SmVideoDataNode tmp = videoDataQueue->recycleNode;
                videoDataQueue->recycleNode = tmp->next;
                smDestroyVideoDataNode(tmp);
            }
        }
        videoDataQueue->numRecycleNode = 0;
    }
}

int smGetVideoDataQueueSize(SmVideoDataQueue videoDataQueue){
    if(videoDataQueue){
        return videoDataQueue->numNode;
    } else{
        return 0;
    }
}

void smVideoDataQueueEnqueueData(SmVideoDataQueue videoDataQueue, SmVideoDataNode videoDataNode){
    if(videoDataQueue && videoDataNode){
        if(!videoDataQueue->fistNode){
            videoDataQueue->fistNode = videoDataNode;
        }
        videoDataQueue->numNode++;
        videoDataQueue->lastNode = videoDataNode;
    }
}


SmVideoDataNode  smVideoDataQueueDequeueData(SmVideoDataQueue videoDataQueue){
    SmVideoDataNode  videoDataNode = NULL;
    if (videoDataQueue && videoDataQueue->fistNode){
        videoDataNode = videoDataQueue->fistNode;
        videoDataQueue->fistNode = videoDataQueue->fistNode->next;
        videoDataQueue->numNode--;
    }
    return videoDataNode;
}

SmVideoDataNode smCreateVideoDataNodeFromCache(SmVideoDataQueue videoDataQueue){
    SmVideoDataNode  videoDataNode = NULL;
    if (videoDataQueue){
        if(videoDataQueue->recycleNode){
            videoDataNode = videoDataQueue->recycleNode;
            videoDataQueue->recycleNode = videoDataQueue->recycleNode->next;
        } else{
            videoDataNode = smCreateVideoDataNode();
            if(videoDataNode){
                videoDataNode->videoData = smCreateVideoData();
            }
        }
    }
    return videoDataNode;
}
void smCacheVideoDataNodeToCache(SmVideoDataQueue videoDataQueue,SmVideoDataNode videoDataNode){
    if(videoDataQueue && videoDataNode){
        if(videoDataQueue->recycleNode){
            videoDataQueue->recycleNode->next = videoDataNode;
        } else{
            videoDataQueue ->recycleNode = videoDataNode;
        }
    }
}