//
// Created by dengjun on 2019/3/20.
//

#include "cloudvoice_blocking_queue.h"
#include "library/CommonLib/thread/cloudvoice_mutex.h"
#include <malloc.h>
#include <library/CommonLib/log/cloudvoice_log.h>

#define MAX_CACHE_FRAME  30

typedef struct ElementNode_* ElementNode;
typedef struct ElementNode_{
    void *element;

    ElementNode next;
}ElementNode_;

struct BlockingQueueEntity_{
    ElementNode firstNode;
    ElementNode lastNode;
    int numNode;
    CloudVoiceMutex clMutex;
    CloudVoiceCond clCond;
    int abortFlag;

}BlockingQueueEntity_;



static void add(CloudVoiceBlockingQueue blockingQueue,void* element){
    if (blockingQueue && element){
        BlockingQueueEntity  queueEntity = blockingQueue->blockingQueueEntity;
        cloudVoiceLockMutex(queueEntity->clMutex);
        if(queueEntity->numNode > MAX_CACHE_FRAME){
            blockingQueue->clear(blockingQueue);
        }

        ElementNode  elementNode = (ElementNode)malloc(sizeof(ElementNode_));
        cloudVoiceLogD("queue add elementNode: %p, element : %p",elementNode,element);
        if (elementNode){
            elementNode->next = NULL;
            elementNode->element = element;

            if(queueEntity->lastNode){
                queueEntity->lastNode->next = elementNode;
            } else{
                queueEntity->lastNode = elementNode;
            }
            queueEntity->numNode++;

            cloudVoiceLogD("queue add 2 elementNode: %p, element : %p",queueEntity->firstNode,element);
            if (!queueEntity->firstNode){
                queueEntity->firstNode = elementNode;
            }
            cloudVoiceLogD("queue add 3 elementNode: %p, element : %p",queueEntity->firstNode,element);
        }

        cloudVoiceUnlockMutex(queueEntity->clMutex);
        cloudVoiceCondSignal(queueEntity->clCond);
    }
}

static void* take(CloudVoiceBlockingQueue blockingQueue){
    void * element = NULL;
    if (blockingQueue ){
        BlockingQueueEntity  queueEntity = blockingQueue->blockingQueueEntity;
        cloudVoiceLockMutex(queueEntity->clMutex);
        cloudVoiceLogD("queueEntity->numNode : %d",queueEntity->numNode);
        if (queueEntity->numNode <=0){
            cloudVoiceCondWait(queueEntity->clCond,queueEntity->clMutex);
        } else{
            ElementNode  elementNode = queueEntity->firstNode;
            cloudVoiceLogD("queue take 1 elementNode: %p, element : %p",elementNode,element);
            if(elementNode){
                element = elementNode->element;
                cloudVoiceLogD("queue take 2  elementNode: %p, element : %p, elementNode->next : %p",elementNode,element,elementNode->next);
                if(queueEntity->firstNode == queueEntity->lastNode){
                    queueEntity->lastNode = NULL;
                    queueEntity->firstNode = NULL;
                } else{
                    queueEntity->firstNode = elementNode->next;
                }
                free(elementNode);
                queueEntity->numNode--;

            }
        }
        cloudVoiceUnlockMutex(queueEntity->clMutex);
    }
    return element;
}

static void clear(CloudVoiceBlockingQueue blockingQueue){
    if (blockingQueue){
        BlockingQueueEntity  queueEntity = blockingQueue->blockingQueueEntity;
        cloudVoiceLockMutex(queueEntity->clMutex);

        while (queueEntity->firstNode){
            ElementNode  elementNode = queueEntity->firstNode;
            queueEntity->firstNode = elementNode->next;
            blockingQueue->freeCallback(blockingQueue,elementNode->element);
            free(elementNode);
        }
        queueEntity->firstNode = NULL;
        queueEntity->lastNode = NULL;
        queueEntity->numNode = 0;

        cloudVoiceUnlockMutex(queueEntity->clMutex);
    }
}

static void destroy(CloudVoiceBlockingQueue blockingQueue){
    if (blockingQueue){
        blockingQueue->clear(blockingQueue);
        BlockingQueueEntity  queueEntity = blockingQueue->blockingQueueEntity;
        free(queueEntity);
        free(blockingQueue);
    }
}




CloudVoiceBlockingQueue cloudVoiceCreateBlockingQueue(FreeCallback elementCallback){
    CloudVoiceBlockingQueue blockingQueue = (CloudVoiceBlockingQueue)malloc(sizeof(CloudVoiceBlockingQueue_));
    if(blockingQueue){
        BlockingQueueEntity blockingQueueEntity = (BlockingQueueEntity)malloc(sizeof(struct BlockingQueueEntity_));
        blockingQueueEntity->firstNode = NULL;
        blockingQueueEntity->lastNode = NULL;
        blockingQueueEntity->numNode = 0;
        blockingQueueEntity->clMutex = cloudVoiceCreateMutex();
        blockingQueueEntity->clCond = cloudVoiceCreateCond();
        blockingQueue->blockingQueueEntity = blockingQueueEntity;

        blockingQueue->add = add;
        blockingQueue->take = take;
        blockingQueue->clear = clear;
        blockingQueue->destroy = destroy;

        blockingQueue->freeCallback = elementCallback;
    }
    return blockingQueue;
}