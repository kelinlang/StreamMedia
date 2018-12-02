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
                                      StreamMedia/video)


add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).


             #公共库开始
            #src/main/cpp/CommonLib/log/cl_sdl_log.c

            #src/main/cpp/CommonLib/thread/cl_sdl_thread.c
            #src/main/cpp/CommonLib/thread/cl_sdl_mutex.c
            # #公共库结束

            # #媒体库
            # src/main/cpp/CommonLib/StreamMedia/sm_egl.c
            # src/main/cpp/CommonLib/StreamMedia/sm_gles2_display.c
            # src/main/cpp/CommonLib/StreamMedia/sm_video_display.c
            # src/main/cpp/CommonLib/StreamMedia/gles2/sm_gles2_utils.c

            # src/main/cpp/CommonLib/StreamMedia/model/sm_video_data.c
            # src/main/cpp/CommonLib/StreamMedia/model/sm_video_fourcc.c

            # src/main/cpp/CommonLib/StreamMedia/video/sm_video_fourcc.c
              #媒体库结束

             src/main/cpp/native-lib.cpp
              src/main/cpp/looper.cpp
              src/main/cpp/StreamMediaNative.cpp
              src/main/cpp/log.c
              src/main/cpp/MediaCodecTest.cpp)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       native-lib

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib}
                       mediandk
                       OpenMAXAL)