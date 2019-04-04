//
// Created by dengjun on 2019/3/21.
//

#include "cloudvoice_list.h"
#include <malloc.h>
#include "library/CommonLib/thread/cloudvoice_mutex.h"

#define MAX_CACHE_FRAME  30

typedef struct ElementNode_* ElementNode;
typedef struct ElementNode_{
    void *element;

    ElementNode next;
}ElementNode_;

struct ListEntity_{
    ElementNode firstNode;
    ElementNode lastNode;
    int numNode;
    CloudVoiceMutex clMutex;
    CloudVoiceCond clCond;
    int abortFlag;

}ListEntity_;


static ElementNode createNode(){
    ElementNode  elementNode = (ElementNode)malloc(sizeof(ElementNode_));
    return elementNode;
}


static void add(CloudVoiceList list,void* element){
    if (list && element){
        ListEntity  queueEntity = list->listEntity;
        cloudVoiceLockMutex(queueEntity->clMutex);
        if(queueEntity->numNode > MAX_CACHE_FRAME){
            list->clear(list);
        }

        ElementNode  elementNode = (ElementNode)malloc(sizeof(ElementNode_));
        if (elementNode){
            elementNode->next = NULL;
            elementNode->element = element;

            if(queueEntity->lastNode){
                queueEntity->lastNode->next = elementNode;
            }
            queueEntity->lastNode = elementNode;
            queueEntity->numNode++;

            if (!queueEntity->firstNode){
                queueEntity->firstNode = elementNode;
            }
        }

        cloudVoiceCondSignal(queueEntity->clCond);
        cloudVoiceUnlockMutex(queueEntity->clMutex);
    }
}

static void* take(CloudVoiceList list){
    void * element = NULL;
    if (list ){
        ListEntity  queueEntity = list->listEntity;
        cloudVoiceLockMutex(queueEntity->clMutex);

        ElementNode  elementNode = queueEntity->firstNode;
        if(elementNode){
            element = elementNode->element;
            queueEntity->firstNode = elementNode->next;
            queueEntity->numNode--;
            free(elementNode);
        }

        cloudVoiceUnlockMutex(queueEntity->clMutex);
    }
    return element;
}

static void clear(CloudVoiceList list){
    if (list){
        ListEntity  queueEntity = list->listEntity;
        cloudVoiceLockMutex(queueEntity->clMutex);

        while (queueEntity->firstNode){
            ElementNode  elementNode = queueEntity->firstNode;
            queueEntity->firstNode = elementNode->next;
            list->freeCallback(list,elementNode->element);
            free(elementNode);
        }
        queueEntity->firstNode = NULL;
        queueEntity->lastNode = NULL;
        queueEntity->numNode = 0;

        cloudVoiceUnlockMutex(queueEntity->clMutex);
    }
}

static void destroy(CloudVoiceList list){
    if (list){
        list->clear(list);
        ListEntity  queueEntity = list->listEntity;
        free(queueEntity);
        free(list);
    }
}




CloudVoiceList cloudVoiceCreateList(FreeNodeCallback elementCallback){
    CloudVoiceList list = (CloudVoiceList)malloc(sizeof(CloudVoiceList_));
    if(list){
        ListEntity listEntity = (ListEntity)malloc(sizeof(struct ListEntity_));
        listEntity->firstNode = NULL;
        listEntity->lastNode = NULL;
        listEntity->numNode = 0;
        listEntity->clMutex = cloudVoiceCreateMutex();
        listEntity->clCond = cloudVoiceCreateCond();
        list->listEntity = listEntity;

        list->add = add;
        list->take = take;
        list->clear = clear;
        list->destroy = destroy;

        list->freeCallback = elementCallback;
    }
    return list;
}