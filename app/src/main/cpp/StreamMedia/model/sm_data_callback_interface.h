//
// Created by dengjun on 2019/1/18.
//

#ifndef STREAMMEDIADEMO_SM_DATA_CALLBACK_INTERFACE_H
#define STREAMMEDIADEMO_SM_DATA_CALLBACK_INTERFACE_H

/**
 * 参数类型
 * 数据
 * 额外参数
 */
typedef void(*DataCallback)(int type,  int dataSize,void * data, void * ext);


#endif //STREAMMEDIADEMO_SM_DATA_CALLBACK_INTERFACE_H
