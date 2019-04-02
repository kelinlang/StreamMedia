//
// Created by dengjun on 2019/3/20.
//

#include "cloudvoice_mutex.h"
#include <malloc.h>
#include <asm/errno.h>

CloudVoiceMutex_ * cloudVoiceCreateMutex(void){
    CloudVoiceMutex_ *mutex;
    mutex = (CloudVoiceMutex_*)malloc(sizeof(CloudVoiceMutex_));
    if (!mutex){
        return NULL;
    }
    if (pthread_mutex_init(&mutex->id,NULL) != 0){
        free(mutex);
        return NULL;
    }
    return mutex;
}

void cloudVoiceDestroyMutex(CloudVoiceMutex_ *mutex){
    if(mutex){
        pthread_mutex_destroy(&mutex->id);
        free(mutex);
    }
}

void cloudVoiceDestroyMutexP(CloudVoiceMutex_ **mutex){
    if(mutex){
        cloudVoiceDestroyMutex(*mutex);
        *mutex = NULL;
    }
}

int cloudVoiceLockMutex(CloudVoiceMutex_ *mutex){
    if (!mutex){
        return -1;
    }
    return pthread_mutex_lock(&mutex->id);
}

int cloudVoiceUnlockMutex(CloudVoiceMutex_ *mutex){
    if (!mutex){
        return -1;
    }
    return pthread_mutex_unlock(&mutex->id);
}



CloudVoiceCond_ *cloudVoiceCreateCond(void){
    CloudVoiceCond_ *cond;
    cond = (CloudVoiceCond_*)malloc(sizeof(CloudVoiceCond_));
    if (!cond){
        return NULL;
    }
    if (pthread_cond_init(&cond->id,NULL) != 0){
        free(cond);
        return NULL;
    }
    return cond;
}

void cloudVoiceDestroyCond(CloudVoiceCond_ *cond){
    if (cond){
        pthread_cond_destroy(&cond->id);
        free(cond);
    }
}
void cloudVoiceDestroyCondP(CloudVoiceCond_ **cond){
    if (cond){
        cloudVoiceDestroyCond(*cond);
        *cond = NULL;
    }
}

int cloudVoiceCondSignal(CloudVoiceCond_ *cond){
    if(!cond){
        return -1;
    }
    return pthread_cond_signal(&cond->id);
}

int cloudVoiceCondBroadcast(CloudVoiceCond_ *cond){
    if(!cond){
        return -1;
    }
    return pthread_cond_broadcast(&cond->id);
}

int cloudVoiceCondWaitTimeout(CloudVoiceCond_ *cond, CloudVoiceMutex_ *mutex, uint32_t ms){
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

int cloudVoiceCondWait(CloudVoiceCond_ *cond, CloudVoiceMutex_ *mutex){
    if (!cond || !mutex)
        return -1;
//    LOGI("cloudVoiceCondWait");
    return pthread_cond_wait(&cond->id, &mutex->id);
}