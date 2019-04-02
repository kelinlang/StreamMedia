//
// Created by dengjun on 2019/3/20.
//

#ifndef VOICELIBDEMO_CLOUDVOICE_MUTEX_H
#define VOICELIBDEMO_CLOUDVOICE_MUTEX_H

#include <stdint.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif


#define SDL_MUTEX_TIMEDOUT  1
#define SDL_MUTEX_MAXWAIT   (~(uint32_t)0)

typedef struct CloudVoiceMutex_* CloudVoiceMutex;
typedef  struct  CloudVoiceMutex_{
    pthread_mutex_t id;
} CloudVoiceMutex_;

CloudVoiceMutex cloudVoiceCreateMutex();
void cloudVoiceDestroyMutex(CloudVoiceMutex mutex);
void cloudVoiceDestroyMutexP(CloudVoiceMutex* mutex);
int cloudVoiceLockMutex(CloudVoiceMutex mutex);
int cloudVoiceUnlockMutex(CloudVoiceMutex mutex);

typedef struct CloudVoiceCond_* CloudVoiceCond;
typedef struct  CloudVoiceCond_{
    pthread_cond_t id;
}CloudVoiceCond_;

CloudVoiceCond_ *cloudVoiceCreateCond(void);
void cloudVoiceDestroyCond(CloudVoiceCond cond);
void cloudVoiceDestroyCondP(CloudVoiceCond *cond);
int cloudVoiceCondSignal(CloudVoiceCond cond);
int cloudVoiceCondBroadcast(CloudVoiceCond cond);
int cloudVoiceCondWaitTimeout(CloudVoiceCond cond, CloudVoiceMutex mutex, uint32_t ms);
int cloudVoiceCondWait(CloudVoiceCond cond, CloudVoiceMutex mutex);

#ifdef __cplusplus
}
#endif


#endif //VOICELIBDEMO_CLOUDVOICE_MUTEX_H
