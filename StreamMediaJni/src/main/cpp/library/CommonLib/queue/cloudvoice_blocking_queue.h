//
// Created by dengjun on 2019/3/20.
//

#ifndef VOICELIBDEMO_CLOUDVOICE_BLOCKING_QUEUE_H
#define VOICELIBDEMO_CLOUDVOICE_BLOCKING_QUEUE_H


//多线程，需要多线程同步
typedef struct CloudVoiceBlockingQueue_* CloudVoiceBlockingQueue;
//用户回调队列的数据，clear的时候不知道具体类型，需要回调给外面free具体数据
typedef void (*FreeCallback)(void* object, void *element);

typedef struct BlockingQueueEntity_* BlockingQueueEntity;

typedef struct CloudVoiceBlockingQueue_{
    BlockingQueueEntity blockingQueueEntity;

    void (*add)(CloudVoiceBlockingQueue self,void *element);
    void*(*take)(CloudVoiceBlockingQueue self);
    void (*clear)(CloudVoiceBlockingQueue self);
    void (*destroy)(CloudVoiceBlockingQueue self);

    FreeCallback freeCallback;
}CloudVoiceBlockingQueue_;

CloudVoiceBlockingQueue cloudVoiceCreateBlockingQueue(FreeCallback elementCallback);



#endif //VOICELIBDEMO_CLOUDVOICE_BLOCKING_QUEUE_H
