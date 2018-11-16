//
// Created by dengjun on 2018/11/15.
//

#ifndef STREAMMEDIADEMO_CL_SDL_THREAD_H
#define STREAMMEDIADEMO_CL_SDL_THREAD_H

#include <stdint.h>
#include <pthread.h>
#include <sys/types.h>


typedef enum {
    SDL_THREAD_PRIORITY_LOW,
    SDL_THREAD_PRIORITY_NORMAL,
    SDL_THREAD_PRIORITY_HIGH
}SdlThreadPriority;

typedef struct SdlThread{
    pthread_t id;
    int (*func)(void *);
    void *data;
    char name[32];
    int retval;
} SdlThread;


SdlThread* sdl_create_thread(SdlThread *thread, int(*func)(void*),void *data, const char*name);
int  sdl_set_thread_priority(SdlThreadPriority sdl_threadPriority);
void sdl_wait_thread(SdlThread* thread, int *status);
void sdl_detach_thread(SdlThread* thread);


#endif //STREAMMEDIADEMO_CL_SDL_THREAD_H
