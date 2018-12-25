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

typedef struct ClThread_* ClThread;
typedef struct ClThread_{
    pthread_t id;
    int (*func)(void *);
    void *data;
    char name[32];
    int retval;
} ClThread_;


ClThread  clCreateThread(ClThread thread, int(*func)(void *), void *data, const char *name);
int  clSetThreadPriority(SdlThreadPriority sdl_threadPriority);
void clWaitThread(ClThread thread, int *status);
void clDetachThread(ClThread thread);


#endif //STREAMMEDIADEMO_CL_SDL_THREAD_H
