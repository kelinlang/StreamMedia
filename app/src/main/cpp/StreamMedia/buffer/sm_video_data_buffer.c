//
// Created by dengjun on 2018/12/25.
//

#include <malloc.h>
#include "sm_video_data_buffer.h"
#include "../../CommonLib/thread/cl_sdl_mutex.h"


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
        videoDataQueue->clMutex = clCreateMutex();

        videoDataQueue->recycleNode = NULL;
        videoDataQueue->numRecycleNode = 0;
        videoDataQueue->recycleClMutex = clCreateMutex();

    }
    return videoDataQueue;
}
void smDestroyVideoDataQueue(SmVideoDataQueue videoDataQueue){
    if (videoDataQueue){
        smVideoDataQueueClearData(videoDataQueue);
        clDestroyMutex(videoDataQueue->clMutex);
        clDestroyMutex(videoDataQueue->recycleClMutex);
        free(videoDataQueue);
    }
}
void smVideoDataQueueClearData(SmVideoDataQueue videoDataQueue){
    if (videoDataQueue){
        clLockMutex(videoDataQueue->clMutex);
        while (videoDataQueue->fistNode){
            SmVideoDataNode tmp = videoDataQueue->fistNode;
            videoDataQueue->fistNode = tmp->next;
            smDestroyVideoDataNode(tmp);
        }
        videoDataQueue->fistNode = NULL;
        videoDataQueue->lastNode = NULL;
        videoDataQueue->numNode = 0;
        clUnlockMutex(videoDataQueue->clMutex);

        clLockMutex(videoDataQueue->recycleClMutex);
        if(videoDataQueue->recycleNode){
            while (videoDataQueue->recycleNode){
                SmVideoDataNode tmp = videoDataQueue->recycleNode;
                videoDataQueue->recycleNode = tmp->next;
                smDestroyVideoDataNode(tmp);
            }
        }
        videoDataQueue->numRecycleNode = 0;
        clLockMutex(videoDataQueue->recycleClMutex);

    }
}

int smGetVideoDataQueueSize(SmVideoDataQueue videoDataQueue){
    int size = 0;
    if(videoDataQueue){
        clLockMutex(videoDataQueue->clMutex);
        size =  videoDataQueue->numNode;
        clUnlockMutex(videoDataQueue->clMutex);
    }
    return size;
}

void smVideoDataQueueEnqueueData(SmVideoDataQueue videoDataQueue, SmVideoDataNode videoDataNode){
    if(videoDataQueue && videoDataNode){
        clLockMutex(videoDataQueue->clMutex);

        if(!videoDataQueue->fistNode){
            videoDataQueue->fistNode = videoDataNode;
        }
        videoDataQueue->numNode++;
        videoDataQueue->lastNode = videoDataNode;

        clUnlockMutex(videoDataQueue->clMutex);
    }
}


SmVideoDataNode  smVideoDataQueueDequeueData(SmVideoDataQueue videoDataQueue){
    SmVideoDataNode  videoDataNode = NULL;
    if (videoDataQueue && videoDataQueue->fistNode){
        clLockMutex(videoDataQueue->clMutex);

        videoDataNode = videoDataQueue->fistNode;
        videoDataQueue->fistNode = videoDataQueue->fistNode->next;
        videoDataQueue->numNode--;

        clUnlockMutex(videoDataQueue->clMutex);
    }
    return videoDataNode;
}

SmVideoDataNode smCreateVideoDataNodeFromCache(SmVideoDataQueue videoDataQueue){
    SmVideoDataNode  videoDataNode = NULL;
    if (videoDataQueue){
        clLockMutex(videoDataQueue->recycleClMutex);

        if(videoDataQueue->recycleNode){
            videoDataNode = videoDataQueue->recycleNode;
            videoDataQueue->recycleNode = videoDataQueue->recycleNode->next;
        } else{
            videoDataNode = smCreateVideoDataNode();
            if(videoDataNode){
                videoDataNode->videoData = smCreateVideoData();
            }
        }

        clUnlockMutex(videoDataQueue->recycleClMutex);
    }
    return videoDataNode;
}
void smCacheVideoDataNodeToCache(SmVideoDataQueue videoDataQueue,SmVideoDataNode videoDataNode){
    if(videoDataQueue && videoDataNode){
        clLockMutex(videoDataQueue->recycleClMutex);

        if(videoDataQueue->recycleNode){
            videoDataQueue->recycleNode->next = videoDataNode;
        } else{
            videoDataQueue ->recycleNode = videoDataNode;
        }

        clUnlockMutex(videoDataQueue->recycleClMutex);
    }
}