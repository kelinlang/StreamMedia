//
// Created by dengjun on 2019/3/21.
//

#ifndef VOICELIBDEMO_CLOUDVOICE_STATUS_CALLBACK_H
#define VOICELIBDEMO_CLOUDVOICE_STATUS_CALLBACK_H

//externHandle: 保存外部的引用，内部不能释放此引用，需由外部引用自己释放

//回调状态
typedef void(*MediaStatusCallback)(void *externHandle,char *id, int  mediaStatus,void *ext);

//回调数据
typedef void(*MediaDataCallback)(void *externHandle,char *id, int type,  void * dataPacket,void *ext);

#endif //VOICELIBDEMO_CLOUDVOICE_STATUS_CALLBACK_H
