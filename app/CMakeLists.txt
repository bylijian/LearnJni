# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

#指定Cmake最低版本

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

#添加一个名字为 native-lib 的 SHARED library ，它最终的 文件名是 libnative-lib.so
#源文件是 src/main/cpp/native-lib.cpp

add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             src/main/cpp/native-lib.cpp )

#添加一个名字为 native-c-lib 的 SHARED library ，它最终的 文件名是 libnative-c-lib.so
#源文件是 src/main/cpp/native-c-lib.c

add_library( # Sets the name of the library.
             native-c-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             src/main/cpp/native-c-lib.c )

#添加一个名字为 native-java-callback 的 SHARED library ，它最终的 文件名是 libnative-java-callback.so
#源文件是 src/main/cpp/native-java-callback.c

add_library( # Sets the name of the library.
             native-java-callback

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             src/main/cpp/native-java-callback.c )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

#Cmake 默认包含了系统的library比如log，使用log只需要找到它，
#并把它的path保存到一个变量即可

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

#指定 native-lib 需要链接的library

target_link_libraries( # Specifies the target library.
                       native-lib
                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )

#指定 native-c-lib需要链接的library

target_link_libraries( # Specifies the target library.
                       native-c-lib
                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )

#指定  native-java-callback 需要链接的library

target_link_libraries( # Specifies the target library.
                       native-java-callback
                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )