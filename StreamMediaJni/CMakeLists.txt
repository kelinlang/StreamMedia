# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

#libRtmp编译不带加密的版本
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DNO_CRYPTO")

#include_directories(CommonLib/common
#                                    CommonLib/container
#                                    CommonLib/log
#                                    CommonLib/thread
#                                    #StreamMedia/display
#                                    # StreamMedia/model
#                                    # StreamMedia/buffer
#                                    # StreamMedia/gles2
#                                    # StreamMedia/gles2/fsh
#                                    # StreamMedia/gles2/vsh
#                                    #  StreamMedia/video
#
#                                      media/librtmp)

include_directories(src/main/cpp)

add_library( # Sets the name of the library.
        StreamMediaLib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).

        #LibRtmp
        src/main/cpp/library/librtmp/amf.c
        src/main/cpp/library/librtmp/hashswf.c
        src/main/cpp/library/librtmp/log.c
        src/main/cpp/library/librtmp/parseurl.c
        src/main/cpp/library/librtmp/rtmp.c

        #公共库开始
        src/main/cpp/library/CommonLib/log/cloudvoice_log.c
        src/main/cpp/library/CommonLib/thread/cloudvoice_mutex.c
        src/main/cpp/library/CommonLib/thread/cloudvoice_thread.c

        src/main/cpp/library/CommonLib/queue/cloudvoice_blocking_queue.c
        src/main/cpp/library/CommonLib/queue/cloudvoice_list.c
        src/main/cpp/library/CommonLib/map/map.c
        src/main/cpp/library/CommonLib/utils/cloudvoice_utils.c


        #src/main/cpp/CommonLib/android/cl_android_jni.c
        #src/main/cpp/CommonLib/android/cl_android_jni_base.c


        # #公共库结束

        # #媒体库
        src/main/cpp/library/media/model/cloudvoice_audio_video_model.c
        src/main/cpp/library/media/cloudvoice_codec_interface.c
        #媒体库结束


        #Android 相关代码库
        src/main/cpp/android/codec/cloudvoide_android_video_codec.c
        src/main/cpp/android/display/ShaderUtils.cpp
        src/main/cpp/android/display/cloudvoice_shader_utils.c
        src/main/cpp/android/display/cloudvoice_android_video_display.c



        #business 相关代码
        src/main/cpp/business/cloudvoice_media_jni.c
        src/main/cpp/business/cloudvoice_media_manager.c

        src/main/cpp/business/client/cloudvoice_pull_stream_player.c
        src/main/cpp/business/client/cloudvoice_push_stream_client.c

        src/main/cpp/business/model/cloudvoice_media_params.c
        src/main/cpp/business/model/cloudvoice_stream_param.c
        src/main/cpp/business/model/cloudvoice_player_param.c

        src/main/cpp/business/rtmp/cloudvoice_rtmp_client.c

        )


#add_library(openssl-crypto STATIC IMPORTED)
#set_target_properties(openssl-crypto
# PROPERTIES IMPORTED_LOCATION
# ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libcrypto.so)

#添加加密工具(md5, base64, des, aes , asa) des加密 part-2
#add_library(openssl-ssl STATIC IMPORTED)
#set_target_properties(openssl-ssl
# PROPERTIES IMPORTED_LOCATION
#  ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libssl.so)

#添加压缩库
#add_library(z SHARED IMPORTED)
#set_target_properties(z
#  PROPERTIES IMPORTED_LOCATION
#${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libz.so)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log
        )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        StreamMediaLib

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}
        android
        mediandk
        OpenMAXAL
        OpenSLES
        GLESv2
        EGL

        #z
        #crypto
        #ssl
        # openssl-crypto
        #openssl-ssl
        )