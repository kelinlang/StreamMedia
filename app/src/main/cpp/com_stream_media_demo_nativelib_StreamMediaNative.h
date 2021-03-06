/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_stream_media_demo_nativelib_StreamMediaNative */

#ifndef _Included_com_stream_media_demo_nativelib_StreamMediaNative
#define _Included_com_stream_media_demo_nativelib_StreamMediaNative
#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <cstdlib>
#include "CommonLib/log/cl_sdl_log.h"
#include "CommonLib/log/cl_sdl_abstract_log.h"
#include "pthread.h"
#include "media/NdkMediaCodec.h"
#include "stdio.h"


/*
 * Class:     com_stream_media_demo_nativelib_StreamMediaNative
 * Method:    setSurface
 * Signature: (Landroid/view/Surface;)V
 */
JNIEXPORT void JNICALL Java_com_stream_media_demo_nativelib_StreamMediaNative_setSurface
  (JNIEnv *, jclass, jobject);

/*
 * Class:     com_stream_media_demo_nativelib_StreamMediaNative
 * Method:    init
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_stream_media_demo_nativelib_StreamMediaNative_init
  (JNIEnv *, jclass);

/*
 * Class:     com_stream_media_demo_nativelib_StreamMediaNative
 * Method:    release
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_stream_media_demo_nativelib_StreamMediaNative_release
  (JNIEnv *, jclass);

/*
 * Class:     com_stream_media_demo_nativelib_StreamMediaNative
 * Method:    pushVideoData
 * Signature: ([BLcom/stream/media/demo/camera/VideoDataInfo;)V
 */
JNIEXPORT void JNICALL Java_com_stream_media_demo_nativelib_StreamMediaNative_pushVideoData
  (JNIEnv *, jclass, jbyteArray, jobject);

#ifdef __cplusplus
}
#endif
#endif
