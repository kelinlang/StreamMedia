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

#include "cl_android_jni_base.h"

/********************
 * Exception Handle
 ********************/

bool cl_android_jni_base_ExceptionCheck__throwAny(JNIEnv *env)
{
    if ((*env)->ExceptionCheck(env)) {
        (*env)->ExceptionDescribe(env);
        return true;
    }

    return false;
}

bool cl_android_jni_base_ExceptionCheck__catchAll(JNIEnv *env)
{
    if ((*env)->ExceptionCheck(env)) {
        (*env)->ExceptionDescribe(env);
        (*env)->ExceptionClear(env);
        return true;
    }

    return false;
}

int cl_android_jni_base_ThrowExceptionOfClass(JNIEnv* env, jclass clazz, const char* msg)
{
    if ((*env)->ThrowNew(env, clazz, msg) != JNI_OK)
        LOGE("%s: Failed: msg: '%s'\n", __func__, msg);

    return 0;
}

int cl_android_jni_base_ThrowException(JNIEnv* env, const char* class_sign, const char* msg)
{
    int ret = -1;

    if (cl_android_jni_base_ExceptionCheck__catchAll(env)) {
        LOGE("pending exception throwed.\n");
    }

    jclass exceptionClass = cl_android_jni_base_FindClass__catchAll(env, class_sign);
    if (exceptionClass == NULL) {
        CL_ANDROID_JNI_BASE_FUNC_FAIL_TRACE();
        
        ret = -1;
        goto fail;
    }

    ret = cl_android_jni_base_ThrowExceptionOfClass(env, exceptionClass, msg);
    if (ret) {
        CL_ANDROID_JNI_BASE_FUNC_FAIL_TRACE();
        goto fail;
    }

    ret = 0;
fail:
    cl_android_jni_base_DeleteLocalRef__p(env, &exceptionClass);
    return ret;
}

int cl_android_jni_base_ThrowIllegalStateException(JNIEnv *env, const char* msg)
{
    return cl_android_jni_base_ThrowException(env, "java/lang/IllegalStateException", msg);
}

/********************
 * References
 ********************/

jclass cl_android_jni_base_NewGlobalRef__catchAll(JNIEnv *env, jobject obj)
{
    jclass obj_global = (*env)->NewGlobalRef(env, obj);
    if (cl_android_jni_base_ExceptionCheck__catchAll(env) || !(obj_global)) {
        CL_ANDROID_JNI_BASE_FUNC_FAIL_TRACE();
        goto fail;
    }

fail:
    return obj_global;
}

void cl_android_jni_base_DeleteLocalRef(JNIEnv *env, jobject obj)
{
    if (!obj)
        return;
    (*env)->DeleteLocalRef(env, obj);
}

void cl_android_jni_base_DeleteLocalRef__p(JNIEnv *env, jobject *obj)
{
    if (!obj)
        return;
    cl_android_jni_base_DeleteLocalRef(env, *obj);
    *obj = NULL;
}

void cl_android_jni_base_DeleteGlobalRef(JNIEnv *env, jobject obj)
{
    if (!obj)
        return;
    (*env)->DeleteGlobalRef(env, obj);
}

void cl_android_jni_base_DeleteGlobalRef__p(JNIEnv *env, jobject *obj)
{
    if (!obj)
        return;
    cl_android_jni_base_DeleteGlobalRef(env, *obj);
    *obj = NULL;
}

void cl_android_jni_base_ReleaseStringUTFChars(JNIEnv *env, jstring str, const char *c_str)
{
    if (!str || !c_str)
        return;
    (*env)->ReleaseStringUTFChars(env, str, c_str);
}

void cl_android_jni_base_ReleaseStringUTFChars__p(JNIEnv *env, jstring str, const char **c_str)
{
    if (!str || !c_str)
        return;
    cl_android_jni_base_ReleaseStringUTFChars(env, str, *c_str);
    *c_str = NULL;
}

/********************
 * Class Load
 ********************/

jclass cl_android_jni_base_FindClass__catchAll(JNIEnv *env, const char *class_sign)
{
    jclass clazz = (*env)->FindClass(env, class_sign);
    if (cl_android_jni_base_ExceptionCheck__catchAll(env) || !(clazz)) {
        CL_ANDROID_JNI_BASE_FUNC_FAIL_TRACE();
        clazz = NULL;
        goto fail;
    }

fail:
    return clazz;
}

jclass cl_android_jni_base_FindClass__asGlobalRef__catchAll(JNIEnv *env, const char *class_sign)
{
    jclass clazz_global = NULL;
    jclass clazz = cl_android_jni_base_FindClass__catchAll(env, class_sign);
    if (!clazz) {
        CL_ANDROID_JNI_BASE_FUNC_FAIL_TRACE1(class_sign);
        goto fail;
    }

    clazz_global = cl_android_jni_base_NewGlobalRef__catchAll(env, clazz);
    if (!clazz_global) {
        CL_ANDROID_JNI_BASE_FUNC_FAIL_TRACE1(class_sign);
        goto fail;
    }

fail:
    cl_android_jni_base_DeleteLocalRef__p(env, &clazz);
    return clazz_global;
}

jmethodID cl_android_jni_base_GetMethodID__catchAll(JNIEnv *env, jclass clazz, const char *method_name, const char *method_sign)
{
    jmethodID method_id = (*env)->GetMethodID(env, clazz, method_name, method_sign);
    if (cl_android_jni_base_ExceptionCheck__catchAll(env) || !method_id) {
        CL_ANDROID_JNI_BASE_FUNC_FAIL_TRACE2(method_name, method_sign);
        method_id = NULL;
        goto fail;
    }

fail:
    return method_id;
}

jmethodID cl_android_jni_base_GetStaticMethodID__catchAll(JNIEnv *env, jclass clazz, const char *method_name, const char *method_sign)
{
    jmethodID method_id = (*env)->GetStaticMethodID(env, clazz, method_name, method_sign);
    if (cl_android_jni_base_ExceptionCheck__catchAll(env) || !method_id) {
        CL_ANDROID_JNI_BASE_FUNC_FAIL_TRACE2(method_name, method_sign);
        method_id = NULL;
        goto fail;
    }

fail:
    return method_id;
}

jfieldID cl_android_jni_base_GetFieldID__catchAll(JNIEnv *env, jclass clazz, const char *field_name, const char *field_sign)
{
    jfieldID field_id = (*env)->GetFieldID(env, clazz, field_name, field_sign);
    if (cl_android_jni_base_ExceptionCheck__catchAll(env) || !field_id) {
        CL_ANDROID_JNI_BASE_FUNC_FAIL_TRACE2(field_name, field_sign);
        field_id = NULL;
        goto fail;
    }

fail:
    return field_id;
}

jfieldID cl_android_jni_base_GetStaticFieldID__catchAll(JNIEnv *env, jclass clazz, const char *field_name, const char *field_sign)
{
    jfieldID field_id = (*env)->GetStaticFieldID(env, clazz, field_name, field_sign);
    if (cl_android_jni_base_ExceptionCheck__catchAll(env) || !field_id) {
        CL_ANDROID_JNI_BASE_FUNC_FAIL_TRACE2(field_name, field_sign);
        field_id = NULL;
        goto fail;
    }

fail:
    return field_id;
}

/********************
 * Misc Functions
 ********************/

jbyteArray cl_android_jni_base_NewByteArray__catchAll(JNIEnv *env, jsize capacity)
{
    jbyteArray local = (*env)->NewByteArray(env, capacity);
    if (cl_android_jni_base_ExceptionCheck__catchAll(env) || !local)
        return NULL;

    return local;
}

jbyteArray cl_android_jni_base_NewByteArray__asGlobalRef__catchAll(JNIEnv *env, jsize capacity)
{
    jbyteArray local = (*env)->NewByteArray(env, capacity);
    if (cl_android_jni_base_ExceptionCheck__catchAll(env) || !local)
        return NULL;

    jbyteArray global = (*env)->NewGlobalRef(env, local);
    cl_android_jni_base_DeleteLocalRef__p(env, &local);
    return global;
}

int cl_android_jni_base_GetSystemAndroidApiLevel(JNIEnv *env)
{
    static int SDK_INT = 0;
    if (SDK_INT > 0)
        return SDK_INT;

//    SDK_INT = cl_android_jni_baseC_android_os_Build__VERSION__SDK_INT__get__catchAll(env);
    LOGI("API-Level: %d\n", SDK_INT);
    return SDK_INT;
}
