//
// Created by dengjun on 2018/11/15.
//

#ifndef STREAMMEDIADEMO_CL_SDL_MUTEX_H
#define STREAMMEDIADEMO_CL_SDL_MUTEX_H

#include <stdint.h>
#include <pthread.h>

#define SDL_MUTEX_TIMEDOUT  1
#define SDL_MUTEX_MAXWAIT   (~(uint32_t)0)

typedef struct ClMutex_* ClMutex;
typedef  struct  ClMutex_{
    pthread_mutex_t id;
} ClMutex_;

ClMutex clCreateMutex();
void clDestroyMutex(ClMutex mutex);
void clDestroyMutexP(ClMutex* mutex);
int clLockMutex(ClMutex mutex);
int clUnlockMutex(ClMutex mutex);


typedef struct  ClCond{
    pthread_cond_t id;
}ClCond;

ClCond *clCreateCond(void);
void clDestroyCond(ClCond *cond);
void clDestroyCondP(ClCond **cond);
int clCondSignal(ClCond *cond);
int clCondBroadcast(ClCond *cond);
int clCondWaitTimeout(ClCond *cond, ClMutex_ *mutex, uint32_t ms);
int clCondWait(ClCond *cond, ClMutex_ *mutex);

#endif //STREAMMEDIADEMO_CL_SDL_MUTEX_H
