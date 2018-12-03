//
// Created by dengjun on 2018/12/3.
//

#ifndef STREAMMEDIADEMO_CL_ANDROID_JNI_H
#define STREAMMEDIADEMO_CL_ANDROID_JNI_H

#include "jni.h"

JavaVM * cl_android_get_javaVm();

JNIEnv* cl_android_jni_attach_current_thread_env();
void cl_android_jni_detach_current_thread_env();

jint    cl_android_jni_SetupThreadEnv(JNIEnv **p_env);
void    cl_android_jni_DetachThreadEnv();


jobject cl_android_jni_NewObjectAsGlobalRef(JNIEnv *env, jclass clazz, jmethodID methodID, ...);
void    cl_android_jni_DeleteGlobalRefP(JNIEnv *env, jobject *obj_ptr);
void    cl_android_jni_DeleteLocalRefP(JNIEnv *env, jobject *obj_ptr);


#endif //STREAMMEDIADEMO_CL_ANDROID_JNI_H
