/*
 * copyright (c) 2015 Zhang Rui <bbcallen@gmail.com>
 *
 * This file is part of jni4android.
 *
 * jni4android is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * jni4android is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with jni4android; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef CL_ANDROID_JNI_BASE_H
#define CL_ANDROID_JNI_BASE_H

#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <jni.h>
#include "../log/cl_sdl_log.h"

#ifndef CL_ANDROID_JNI_BASE_UNUSED
#define CL_ANDROID_JNI_BASE_UNUSED(x) x __attribute__((unused))
#endif


#define CL_ANDROID_JNI_BASE_FUNC_FAIL_TRACE()               do {LOGE("%s: failed\n", __func__);} while (0)
#define CL_ANDROID_JNI_BASE_FUNC_FAIL_TRACE1(x__)           do {LOGE("%s: failed: %s\n", __func__, x__);} while (0)
#define CL_ANDROID_JNI_BASE_FUNC_FAIL_TRACE2(x1__, x2__)    do {LOGE("%s: failed: %s %s\n", __func__, x1__, x2__);} while (0)

#define CL_ANDROID_JNI_BASE_LOAD_CLASS(class__) \
    do { \
        ret = cl_android_jni_base_loadClass__cl_android_jni_baseC_##class__(env); \
        if (ret) \
            goto fail; \
    } while (0)

/********************
 * Exception Handle
 ********************/

bool cl_android_jni_base_ExceptionCheck__throwAny(JNIEnv *env);
bool cl_android_jni_base_ExceptionCheck__catchAll(JNIEnv *env);
int  cl_android_jni_base_ThrowExceptionOfClass(JNIEnv* env, jclass clazz, const char* msg);
int  cl_android_jni_base_ThrowException(JNIEnv* env, const char* class_sign, const char* msg);
int  cl_android_jni_base_ThrowIllegalStateException(JNIEnv *env, const char* msg);

/********************
 * References
 ********************/
jclass cl_android_jni_base_NewGlobalRef__catchAll(JNIEnv *env, jobject obj);
void   cl_android_jni_base_DeleteLocalRef(JNIEnv *env, jobject obj);
void   cl_android_jni_base_DeleteLocalRef__p(JNIEnv *env, jobject *obj);
void   cl_android_jni_base_DeleteGlobalRef(JNIEnv *env, jobject obj);
void   cl_android_jni_base_DeleteGlobalRef__p(JNIEnv *env, jobject *obj);

void   cl_android_jni_base_ReleaseStringUTFChars(JNIEnv *env, jstring str, const char *c_str);
void   cl_android_jni_base_ReleaseStringUTFChars__p(JNIEnv *env, jstring str, const char **c_str);

/********************
 * Class Load
 ********************/

int    cl_android_jni_base_LoadAll__catchAll(JNIEnv *env);
jclass cl_android_jni_base_FindClass__catchAll(JNIEnv *env, const char *class_sign);
jclass cl_android_jni_base_FindClass__asGlobalRef__catchAll(JNIEnv *env, const char *class_sign);

jmethodID cl_android_jni_base_GetMethodID__catchAll(JNIEnv *env, jclass clazz, const char *method_name, const char *method_sign);
jmethodID cl_android_jni_base_GetStaticMethodID__catchAll(JNIEnv *env, jclass clazz, const char *method_name, const char *method_sign);

jfieldID cl_android_jni_base_GetFieldID__catchAll(JNIEnv *env, jclass clazz, const char *field_name, const char *method_sign);
jfieldID cl_android_jni_base_GetStaticFieldID__catchAll(JNIEnv *env, jclass clazz, const char *field_name, const char *method_sign);

/********************
 * Misc Functions
 ********************/

jbyteArray cl_android_jni_base_NewByteArray__catchAll(JNIEnv *env, jsize capacity);
jbyteArray cl_android_jni_base_NewByteArray__asGlobalRef__catchAll(JNIEnv *env, jsize capacity);

int cl_android_jni_base_GetSystemAndroidApiLevel(JNIEnv *env);

#endif//cl_android_jni_base_INTERNAL_H
