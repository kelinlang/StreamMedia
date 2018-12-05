//
// Created by dengjun on 2018/12/3.
//

#include <pthread.h>
#include <unistd.h>
#include "cl_android_jni.h"
#include "../log/cl_sdl_log.h"
#include "cl_android_jni_base.h"

static JavaVM * g_jvm;
static pthread_key_t g_thread_key;
static pthread_once_t g_key_once = PTHREAD_ONCE_INIT;

JavaVM * cl_android_get_javaVm(){
    return g_jvm;
}

static void SDL_JNI_ThreadDestroyed(void* value)
{
    JNIEnv *env = (JNIEnv*) value;
    if (env != NULL) {
        LOGE("%s: [%d] didn't call SDL_JNI_DetachThreadEnv() explicity\n", __func__, (int)gettid());
        (*g_jvm)->DetachCurrentThread(g_jvm);
        pthread_setspecific(g_thread_key, NULL);
    }
}


static void make_thread_key()
{
    pthread_key_create(&g_thread_key, SDL_JNI_ThreadDestroyed);
}




JNIEnv* cl_android_jni_attach_current_thread_env(){
    JNIEnv *env = NULL;
    if (!g_jvm){
        return NULL;
    }

    if ((*g_jvm)->AttachCurrentThread(g_jvm, &env,NULL) != 0)
    {
        LOGI("Failed to attach current thread");
    }
    return env;
}
void cl_android_jni_detach_current_thread_env(){
    if (g_jvm){
        (*g_jvm)->DetachCurrentThread(g_jvm);
    }
}

jint    cl_android_jni_SetupThreadEnv(JNIEnv **p_env){
    if(!g_jvm){
        LOGE("SDL_JNI_GetJvm: AttachCurrentThread: NULL g_jvm");
        return -1;
    }

    pthread_once(&g_key_once,make_thread_key);

    JNIEnv *env = (JNIEnv*) pthread_getspecific(g_thread_key);
    if (env) {
        *p_env = env;
        return 0;
    }

    if ((*g_jvm)->AttachCurrentThread(g_jvm, &env, NULL) == JNI_OK) {
        pthread_setspecific(g_thread_key, env);
        *p_env = env;
        return 0;
    }

    return -1;
}



void    cl_android_jni_DetachThreadEnv(){
    JavaVM *jvm = g_jvm;

    LOGI("%s: [%d]\n", __func__, (int)gettid());

    pthread_once(&g_key_once, make_thread_key);

    JNIEnv *env = pthread_getspecific(g_thread_key);
    if (!env)
        return;
    pthread_setspecific(g_thread_key, NULL);

    if ((*jvm)->DetachCurrentThread(env) == JNI_OK)
        return;

    return;
}


jobject cl_android_jni_NewObjectAsGlobalRef(JNIEnv *env, jclass clazz, jmethodID methodID, ...){
    va_list args;
    va_start(args, methodID);

    jobject global_object = NULL;
    jobject local_object = (*env)->NewObjectV(env, clazz, methodID, args);
    if (!cl_android_jni_base_ExceptionCheck__throwAny(env) && local_object) {
        global_object = (*env)->NewGlobalRef(env, local_object);
        cl_android_jni_DeleteLocalRefP(env, &local_object);
    }

    va_end(args);
    return global_object;
}


void    cl_android_jni_DeleteGlobalRefP(JNIEnv *env, jobject *obj_ptr){
    if (!obj_ptr || !*obj_ptr)
        return;

    (*env)->DeleteGlobalRef(env, *obj_ptr);
    *obj_ptr = NULL;
}

void    cl_android_jni_DeleteLocalRefP(JNIEnv *env, jobject *obj_ptr){
    if (!obj_ptr || !*obj_ptr)
        return;

    (*env)->DeleteLocalRef(env, *obj_ptr);
    *obj_ptr = NULL;
}



JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
    LOGI("---------------------JNI_OnLoad start------------------------");



    int retval;
    JNIEnv* env = NULL;

    g_jvm = vm;
    if ((*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_6) != JNI_OK) {
        LOGI("---------------------JNI_OnLoad error------------------------");
        return JNI_ERR;
    }


    //此处可以动态注册本地方法

    LOGI("---------------------JNI_OnLoad finish------------------------");

    return JNI_VERSION_1_6;
}