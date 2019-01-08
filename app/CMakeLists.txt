# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.


include_directories(CommonLib/common
                                    CommonLib/container
                                    CommonLib/log
                                    CommonLib/thread
                                    StreamMedia/display
                                     StreamMedia/model
                                     StreamMedia/buffer
                                     StreamMedia/gles2
                                     StreamMedia/gles2/fsh
                                     StreamMedia/gles2/vsh
                                      StreamMedia/video)


add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).


             #公共库开始
            src/main/cpp/CommonLib/log/cl_sdl_log.c
            src/main/cpp/CommonLib/log/cl_sdl_abstract_log.c

            src/main/cpp/CommonLib/thread/cl_sdl_thread.c
            src/main/cpp/CommonLib/thread/cl_sdl_mutex.c


            src/main/cpp/CommonLib/android/cl_android_jni.c
            src/main/cpp/CommonLib/android/cl_android_jni_base.c


            # #公共库结束

            # #媒体库
            src/main/cpp/StreamMedia/model/sm_video_data.c

            src/main/cpp/StreamMedia/display/sm_egl.c
            src/main/cpp/StreamMedia/display/sm_gles2_display.c
            src/main/cpp/StreamMedia/display/sm_video_display.c

            src/main/cpp/StreamMedia/display/gles2/sm_gles2_utils.c
            src/main/cpp/StreamMedia/display/gles2/gles2_internal.c
            src/main/cpp/StreamMedia/display/gles2/shader.c
            src/main/cpp/StreamMedia/display/gles2/color.c

            src/main/cpp/StreamMedia/display/gles2/fsh/rgb.fsh.c
            src/main/cpp/StreamMedia/display/gles2/fsh/yuv420p.fsh.c
            src/main/cpp/StreamMedia/display/gles2/fsh/yuv420sp.fsh.c
            src/main/cpp/StreamMedia/display/gles2/fsh/yuv444p10le.fsh.c
            src/main/cpp/StreamMedia/display/gles2/vsh/mvp.vsh.c

            src/main/cpp/StreamMedia/video/sm_video_codec.c


            src/main/cpp/StreamMedia/buffer/sm_video_data_buffer.c


            src/main/cpp/StreamMedia/audio/sm_audio_data.c
            src/main/cpp/StreamMedia/audio/sm_audio_record.c
            src/main/cpp/StreamMedia/audio/sm_opensles_record.c
              #媒体库结束

             src/main/cpp/native-lib.cpp
              src/main/cpp/looper.cpp
              src/main/cpp/StreamMediaNative.cpp

              src/main/cpp/MediaCodecTest.cpp


              src/main/cpp/test/test_opengles.c
              #src/main/cpp/test/test_opengles_cpp.cpp
              src/main/cpp/test/ShaderUtils.cpp


              src/main/cpp/test/test_opensles.cpp)

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
                       native-lib

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib}
                       android
                       mediandk
                       OpenMAXAL
                       OpenSLES
                       GLESv2
                       EGL)