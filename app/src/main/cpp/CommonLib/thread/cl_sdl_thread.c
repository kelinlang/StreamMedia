//
// Created by dengjun on 2018/11/15.
//

#include <unistd.h>
#include <string.h>
#include "cl_sdl_thread.h"
#include "../log/cl_sdl_log.h"

static void * sdl_func(void *data){
    ClThread_ *thread = data;
    LOGI("SDL_RunThread: [%d] %s\n", (int)gettid(), thread->name);
    pthread_setname_np(pthread_self(),thread->name);
    thread->retval = thread->func(thread->data);

    return NULL;
}

ClThread  clCreateThread(int(*func)(void *), void *data, const char *name){
    ClThread thread = (ClThread)malloc(sizeof(ClThread_));
    if (thread){
        thread->func = func;
        thread->data = data;
        strlcpy(thread->name,name, sizeof(thread->name)-1);
        int retval = pthread_create(&thread->id,NULL,sdl_func,thread);
        if(retval){
            return NULL;
        }
    }
    return thread;
}

int  clSetThreadPriority(SdlThreadPriority sdl_threadPriority){
    struct sched_param sched;
    int policy;
    pthread_t thread = pthread_self();

    if (pthread_getschedparam(thread, &policy, &sched) < 0) {
        LOGE("pthread_getschedparam() failed");
        return -1;
    }
    if (sdl_threadPriority == SDL_THREAD_PRIORITY_LOW) {
        sched.sched_priority = sched_get_priority_min(policy);
    } else if (sdl_threadPriority == SDL_THREAD_PRIORITY_HIGH) {
        sched.sched_priority = sched_get_priority_max(policy);
    } else {
        int min_priority = sched_get_priority_min(policy);
        int max_priority = sched_get_priority_max(policy);
        sched.sched_priority = (min_priority + (max_priority - min_priority) / 2);
    }
    if (pthread_setschedparam(thread, policy, &sched) < 0) {
        LOGE("pthread_setschedparam() failed");
        return -1;
    }
    return 0;

}

void clWaitThread(ClThread thread, int *status){
    if(!thread){
        return;
    }
    pthread_join(thread->id,NULL);
    if (status){
        *status = thread->retval;
    }
}

void clDetachThread(ClThread_ *thread){
    if (!thread){
        return;
    }
    pthread_detach(thread->id);
}

void clReleaseThread(ClThread thread){
    if(thread){
        free(thread);
    }
}
