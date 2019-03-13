/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_medialib_video_OpenGlEs */

#ifndef _Included_com_medialib_video_OpenGlEs
#define _Included_com_medialib_video_OpenGlEs
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_medialib_video_OpenGlEs
 * Method:    setSurface
 * Signature: (Landroid/view/Surface;)I
 */
JNIEXPORT jint JNICALL Java_com_medialib_video_OpenGlEs_setSurface
  (JNIEnv *, jobject, jobject);

/*
 * Class:     com_medialib_video_OpenGlEs
 * Method:    setMatrix
 * Signature: ([F)I
 */
JNIEXPORT jint JNICALL Java_com_medialib_video_OpenGlEs_setMatrix
  (JNIEnv *, jobject, jfloatArray);

/*
 * Class:     com_medialib_video_OpenGlEs
 * Method:    start
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_medialib_video_OpenGlEs_start
  (JNIEnv *, jobject);

/*
 * Class:     com_medialib_video_OpenGlEs
 * Method:    stop
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_medialib_video_OpenGlEs_stop
  (JNIEnv *, jobject);

/*
 * Class:     com_medialib_video_OpenGlEs
 * Method:    sendYuvData
 * Signature: (I[BI)V
 */
JNIEXPORT void JNICALL Java_com_medialib_video_OpenGlEs_sendYuvData
  (JNIEnv *, jobject, jint, jbyteArray, jint);

/*
 * Class:     com_medialib_video_OpenGlEs
 * Method:    sendData
 * Signature: (I[BI)V
 */
JNIEXPORT void JNICALL Java_com_medialib_video_OpenGlEs_sendData
  (JNIEnv *, jobject, jint, jbyteArray, jint);

/*
 * Class:     com_medialib_video_OpenGlEs
 * Method:    sendVideoData
 * Signature: (Lcom/stream/media/demo/camera/VideoData;)V
 */
JNIEXPORT void JNICALL Java_com_medialib_video_OpenGlEs_sendVideoData
  (JNIEnv *, jobject, jobject);

#ifdef __cplusplus
}
#endif
#endif
