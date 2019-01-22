//
// Created by dengjun on 2018/11/15.
//

#include <malloc.h>
#include <asm/errno.h>
#include "cl_sdl_mutex.h"
#include "../log/cl_sdl_log.h"


ClMutex_ * clCreateMutex(void){
    ClMutex_ *mutex;
    mutex = (ClMutex_*)malloc(sizeof(ClMutex_));
    if (!mutex){
        return NULL;
    }
    if (pthread_mutex_init(&mutex->id,NULL) != 0){
        free(mutex);
        return NULL;
    }
    return mutex;
}

void clDestroyMutex(ClMutex_ *mutex){
    if(mutex){
        pthread_mutex_destroy(&mutex->id);
        free(mutex);
    }
}

void clDestroyMutexP(ClMutex_ **mutex){
    if(mutex){
        clDestroyMutex(*mutex);
        *mutex = NULL;
    }
}

int clLockMutex(ClMutex_ *mutex){
    if (!mutex){
        return -1;
    }
    return pthread_mutex_lock(&mutex->id);
}

int clUnlockMutex(ClMutex_ *mutex){
    if (!mutex){
        return -1;
    }
    return pthread_mutex_unlock(&mutex->id);
}



ClCond_ *clCreateCond(void){
    ClCond_ *cond;
    cond = (ClCond_*)malloc(sizeof(ClCond_));
    if (!cond){
        return NULL;
    }
    if (pthread_cond_init(&cond->id,NULL) != 0){
        free(cond);
        return NULL;
    }
    return cond;
}

void clDestroyCond(ClCond_ *cond){
    if (cond){
        pthread_cond_destroy(&cond->id);
        free(cond);
    }
}
void clDestroyCondP(ClCond_ **cond){
    if (cond){
        clDestroyCond(*cond);
        *cond = NULL;
    }
}

int clCondSignal(ClCond_ *cond){
    if(!cond){
        return -1;
    }
    return pthread_cond_signal(&cond->id);
}

int clCondBroadcast(ClCond_ *cond){
    if(!cond){
        return -1;
    }
    return pthread_cond_broadcast(&cond->id);
}

int clCondWaitTimeout(ClCond_ *cond, ClMutex_ *mutex, uint32_t ms){
    int retval;
    struct timeval delta;
    struct timespec abstime;

    if (!cond || !mutex)
        return -1;

    gettimeofday(&delta,NULL);
    abstime.tv_sec = delta.tv_sec + (ms/1000);
    abstime.tv_nsec = (delta.tv_usec + (ms %1000)*1000)*1000;
    if (abstime.tv_nsec > 1000000000) {
        abstime.tv_sec += 1;
        abstime.tv_nsec -= 1000000000;
    }

    while (1) {
        retval = pthread_cond_timedwait(&cond->id, &mutex->id, &abstime);
        if (retval == 0)
            return 0;
        else if (retval == EINTR)
            continue;
        else if (retval == ETIMEDOUT)
            return SDL_MUTEX_TIMEDOUT;
        else
            break;
    }

    return -1;
}

int clCondWait(ClCond_ *cond, ClMutex_ *mutex){
    if (!cond || !mutex)
        return -1;
//    LOGI("clCondWait");
    return pthread_cond_wait(&cond->id, &mutex->id);
}