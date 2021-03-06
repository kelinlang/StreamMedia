/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_medialib_audio_base_OpenslEs */

#ifndef _Included_com_medialib_audio_base_OpenslEs
#define _Included_com_medialib_audio_base_OpenslEs
#ifdef __cplusplus
extern "C" {
#endif
#include "../CommonLib/log/cl_sdl_log.h"
#include "../CommonLib/android/cl_android_jni.h"
/*
 * Class:     com_medialib_audio_base_OpenslEs
 * Method:    process_opensles_playstart
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_medialib_audio_base_OpenslEs_process_1opensles_1playstart
  (JNIEnv *, jobject);

/*
 * Class:     com_medialib_audio_base_OpenslEs
 * Method:    process_opensles_playstop
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_medialib_audio_base_OpenslEs_process_1opensles_1playstop
  (JNIEnv *, jobject);

/*
 * Class:     com_medialib_audio_base_OpenslEs
 * Method:    process_opensles_putplaydata
 * Signature: ([SI)V
 */
JNIEXPORT void JNICALL Java_com_medialib_audio_base_OpenslEs_process_1opensles_1putplaydata
  (JNIEnv *, jobject, jshortArray, jint);

#ifdef __cplusplus
}
#endif
#endif
