//
// Created by dengjun on 2018/11/15.
//

#ifndef STREAMMEDIADEMO_CL_SDL_MUTEX_H
#define STREAMMEDIADEMO_CL_SDL_MUTEX_H

#include <stdint.h>
#include <pthread.h>

#define SDL_MUTEX_TIMEDOUT  1
#define SDL_MUTEX_MAXWAIT   (~(uint32_t)0)

typedef  struct  SdlMutex{
    pthread_mutex_t id;
} SdlMutex;

SdlMutex * sdl_create_mutex(void);
void sdl_destroy_mutex(SdlMutex *mutex);
void sdl_destroy_mutex_p(SdlMutex **mutex);
int sdl_lock_mutex(SdlMutex *mutex);
int sdl_unlock_mutex(SdlMutex *mutex);


typedef struct  SdlCond{
    pthread_cond_t id;
}SdlCond;

SdlCond *sdl_create_cond(void);
void sdl_destroy_cond(SdlCond *cond);
void sdl_destroy_cond_p(SdlCond **cond);
int sdl_cond_signal(SdlCond *cond);
int sdl_cond_broadcast(SdlCond *cond);
int sdl_cond_wait_timeout(SdlCond *cond,SdlMutex *mutex, uint32_t ms);
int sdl_cond_wait(SdlCond *cond, SdlMutex *mutex);

#endif //STREAMMEDIADEMO_CL_SDL_MUTEX_H
