//
// Created by dengjun on 2019/4/24.
//

#include "com_stream_media_demo_jni_JniTest.h"

extern "C" {
#include "Tree.h"
}


JNIEXPORT void JNICALL Java_com_stream_media_demo_jni_JniTest_start
        (JNIEnv *env, jobject jobject1) {
    BiTree biTree;
    createBiTree(&biTree);
//    PreOrderTraverse(biTree);
//    PreOrderTraverseStack(biTree);
//    INOrderTraverse(biTree);
    INOrderTraverseStack(biTree);
    INOrderTraverseStack2(biTree);
    PostOrderTraverse(biTree);
    PostOrderTraverseStack(biTree);
}

/*
 * Class:     com_stream_media_demo_jni_JniTest
 * Method:    stop
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_stream_media_demo_jni_JniTest_stop
        (JNIEnv *env, jobject jobject1) {

}