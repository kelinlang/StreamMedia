//
// Created by dengjun on 2018/11/15.
//

#include <malloc.h>
#include <asm/errno.h>
#include "cl_sdl_mutex.h"


SdlMutex * sdl_create_mutex(void){
    SdlMutex *mutex;
    mutex = (SdlMutex*)malloc(sizeof(SdlMutex));
    if (!mutex){
        return NULL;
    }
    if (pthread_mutex_init(&mutex->id,NULL) != 0){
        free(mutex);
        return NULL;
    }
    return mutex;
}

void sdl_destroy_mutex(SdlMutex *mutex){
    if(mutex){
        pthread_mutex_destroy(&mutex->id);
        free(mutex);
    }
}

void sdl_destroy_mutex_p(SdlMutex **mutex){
    if(mutex){
        sdl_destroy_mutex(*mutex);
        *mutex = NULL;
    }
}

int sdl_lock_mutex(SdlMutex *mutex){
    if (!mutex){
        return -1;
    }
    return pthread_mutex_lock(&mutex->id);
}

int sdl_unlock_mutex(SdlMutex *mutex){
    if (!mutex){
        return -1;
    }
    return pthread_mutex_unlock(&mutex->id);
}



SdlCond *sdl_create_cond(void){
    SdlCond *cond;
    cond = (SdlCond*)malloc(sizeof(SdlCond));
    if (!cond){
        return NULL;
    }
    if (pthread_cond_init(&cond->id,NULL) != 0){
        free(cond);
        return NULL;
    }
    return cond;
}

void sdl_destroy_cond(SdlCond *cond){
    if (cond){
        pthread_cond_destroy(&cond->id);
        free(cond);
    }
}
void sdl_destroy_cond_p(SdlCond **cond){
    if (cond){
        sdl_destroy_cond(*cond);
        *cond = NULL;
    }
}

int sdl_cond_signal(SdlCond *cond){
    if(!cond){
        return -1;
    }
    return pthread_cond_signal(&cond->id);
}

int sdl_cond_broadcast(SdlCond *cond){
    if(!cond){
        return -1;
    }
    return pthread_cond_broadcast(&cond->id);
}

int sdl_cond_wait_timeout(SdlCond *cond,SdlMutex *mutex, uint32_t ms){
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

int sdl_cond_wait(SdlCond *cond, SdlMutex *mutex){
    if (!cond || !mutex)
        return -1;

    return pthread_cond_wait(&cond->id, &mutex->id);
}