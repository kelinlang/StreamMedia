//
// Created by dengjun on 2018/12/25.
//

#include <malloc.h>
#include "sm_video_data_buffer.h"
#include "../../CommonLib/thread/cl_sdl_mutex.h"
#include "../../CommonLib/log/cl_sdl_log.h"

 SmVideoDataNode smCreateVideoDataNode(){
    SmVideoDataNode videoDataNode = (SmVideoDataNode)malloc(sizeof(struct  SmVideoDataNode_));
    if (videoDataNode != NULL){
        videoDataNode->videoData = smCreateVideoData();
        videoDataNode->serial = 0;
        videoDataNode->next = NULL;
    }
    return videoDataNode;
}

void smDestoryVideoDataNode(SmVideoDataNode videoDataNode){
    if(videoDataNode){
        if(videoDataNode->videoData){
            smDestroyVideoData(videoDataNode->videoData);
        }
        free(videoDataNode);
    }
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
        videoDataQueue->firstNode = NULL;
        videoDataQueue->lastNode = NULL;
        videoDataQueue->numNode = 0;
        videoDataQueue->clMutex = clCreateMutex();
        videoDataQueue->clCond = clCreateCond();

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
        clDestroyCond(videoDataQueue->clCond);
        free(videoDataQueue);
    }
}
void smVideoDataQueueClearData(SmVideoDataQueue videoDataQueue){
    if (videoDataQueue){
        clLockMutex(videoDataQueue->clMutex);
        while (videoDataQueue->firstNode){
            SmVideoDataNode tmp = videoDataQueue->firstNode;
            videoDataQueue->firstNode = tmp->next;
            smDestroyVideoDataNode(tmp);
        }
        videoDataQueue->firstNode = NULL;
        videoDataQueue->lastNode = NULL;
        videoDataQueue->numNode = 0;
        clUnlockMutex(videoDataQueue->clMutex);
//
//
        clLockMutex(videoDataQueue->recycleClMutex);
        if(videoDataQueue->recycleNode){
            while (videoDataQueue->recycleNode){
                SmVideoDataNode tmp = videoDataQueue->recycleNode;
                videoDataQueue->recycleNode = tmp->next;
                smDestroyVideoDataNode(tmp);
            }
        }
        videoDataQueue->numRecycleNode = 0;
        clUnlockMutex(videoDataQueue->recycleClMutex);

//        clUnlockMutex(videoDataQueue->clMutex);
//        clDestroyCond(videoDataQueue->clCond);
    }
}

void smVideoDataQueueAbort(SmVideoDataQueue videoDataQueue){
    if(videoDataQueue){
        clLockMutex(videoDataQueue->clMutex);
        videoDataQueue->abortFlag = 1;
        clCondSignal(videoDataQueue->clCond);
        clUnlockMutex(videoDataQueue->clMutex);
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

    if (videoDataNode && videoDataQueue->numNode > MAX_CACHE_FRAME){
        LOGI("cache num node big than max cache num,clear");
        smVideoDataQueueClearData(videoDataQueue);
        LOGI("cache num node big than max cache num,clear finish");
    }

    if(videoDataQueue && videoDataNode){
        clLockMutex(videoDataQueue->clMutex);
//        LOGI("Enqueue  get lock");
        if(videoDataQueue->lastNode){
            videoDataQueue->lastNode->next = videoDataNode;
        }
        videoDataQueue->lastNode = videoDataNode;
        videoDataQueue->numNode++;


        if(!videoDataQueue->firstNode){
            videoDataQueue->firstNode = videoDataNode;
        }

//        LOGI("notify");
        clCondSignal(videoDataQueue->clCond);
        clUnlockMutex(videoDataQueue->clMutex);

    }
}


SmVideoDataNode  smVideoDataQueueDequeueData(SmVideoDataQueue videoDataQueue){
    SmVideoDataNode  videoDataNode = NULL;
    if (videoDataQueue){
        clLockMutex(videoDataQueue->clMutex);
        LOGI("smVideoDataQueueDequeueData start  1  numNode : %d",videoDataQueue->numNode);
        if (videoDataQueue->numNode <= 0){
            LOGD("wait start");
            clCondWait(videoDataQueue->clCond,videoDataQueue->clMutex);
            LOGD("wait end");
        }
//        LOGI("smVideoDataQueueDequeueData start  2");

        videoDataNode = videoDataQueue->firstNode;
        if(videoDataNode){
            videoDataNode->next = NULL;
            videoDataQueue->firstNode = videoDataQueue->firstNode->next;
            videoDataQueue->numNode--;
        }
//        LOGI("smVideoDataQueueDequeueData start  3");
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
            videoDataQueue->recycleNode = videoDataNode->next;
            videoDataNode->next = NULL;
        } else{
            videoDataNode = smCreateVideoDataNode();
            if(videoDataNode){
                videoDataNode->videoData = smCreateVideoData();
            }
        }
        LOGD("%s   videoDataNode : %d",__FUNCTION__,videoDataNode);
        clUnlockMutex(videoDataQueue->recycleClMutex);
    }
    return videoDataNode;
}

void smCacheVideoDataNodeToCache(SmVideoDataQueue videoDataQueue,SmVideoDataNode videoDataNode){
    if(videoDataQueue && videoDataNode){
        clLockMutex(videoDataQueue->recycleClMutex);
        if(videoDataQueue->recycleNode){
            videoDataNode->next = videoDataQueue->recycleNode;
            videoDataQueue->recycleNode = videoDataNode;
        } else{
            videoDataQueue ->recycleNode = videoDataNode;
        }

        clUnlockMutex(videoDataQueue->recycleClMutex);
    }
}

