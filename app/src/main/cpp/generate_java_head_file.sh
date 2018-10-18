#!/usr/bin/env bash
# packagename.classname
TARGET_CLASS_NAME="com.stream.media.demo.nativelib.StreamMediaNative"

PROJECT_PATH=`cd ../ && pwd`

SOURCE_PATH="${PROJECT_PATH}/java"
TARGET_PATH="${PROJECT_PATH}/cpp"

cd "${SOURCE_PATH}"
javah -v -jni -d ${TARGET_PATH} -classpath "${SOURCE_PATH}" "${TARGET_CLASS_NAME}"
