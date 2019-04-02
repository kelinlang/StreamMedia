//
// Created by dengjun on 2019/3/20.
//

#ifndef VOICELIBDEMO_CLOUDVOICE_THREAD_H
#define VOICELIBDEMO_CLOUDVOICE_THREAD_H

#include <stdint.h>
#include <pthread.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
    SDL_THREAD_PRIORITY_LOW,
    SDL_THREAD_PRIORITY_NORMAL,
    SDL_THREAD_PRIORITY_HIGH
}SdlThreadPriority;

typedef struct CloudVoiceThread_* CloudVoiceThread;
typedef struct CloudVoiceThread_{
    pthread_t id;
    int (*func)(void *);
    void *data;
    char name[32];
    int retval;
} CloudVoiceThread_;


CloudVoiceThread  cloudVoiceCreateThread(int(*func)(void *), void *data, const char *name);
int  cloudVoiceSetThreadPriority(SdlThreadPriority sdl_threadPriority);
void cloudVoiceWaitThread(CloudVoiceThread thread, int *status);
void cloudVoiceDetachThread(CloudVoiceThread thread);
void cloudVoiceReleaseThread(CloudVoiceThread thread);

#ifdef __cplusplus
}
#endif

#endif //VOICELIBDEMO_CLOUDVOICE_THREAD_H
