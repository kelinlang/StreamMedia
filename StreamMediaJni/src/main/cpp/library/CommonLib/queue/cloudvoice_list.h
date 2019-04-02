//
// Created by dengjun on 2019/3/21.
//

#ifndef VOICELIBDEMO_CLOUDVOICE_LIST_H
#define VOICELIBDEMO_CLOUDVOICE_LIST_H

//多线程，需要多线程同步
typedef struct CloudVoiceList_* CloudVoiceList;
//用户回调队列的数据，clear的时候不知道具体类型，需要回调给外面free具体数据
typedef void (*FreeCallback)(CloudVoiceList self, void *element);
typedef struct ListEntity_* ListEntity;

typedef struct CloudVoiceList_{
    ListEntity listEntity;

    void (*add)(CloudVoiceList self,void *element);
    void*(*take)(CloudVoiceList self);
    void (*clear)(CloudVoiceList self);
    void (*destroy)(CloudVoiceList self);

    FreeCallback freeCallback;
}CloudVoiceList_;

CloudVoiceList cloudVoiceCreateList(FreeCallback elementCallback);

#endif //VOICELIBDEMO_CLOUDVOICE_LIST_H
