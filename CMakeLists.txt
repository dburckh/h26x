# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html.
# For more examples on how to use CMake, see https://github.com/android/ndk-samples.

# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.22.1)

# Declares the project name. The project name can be accessed via ${ PROJECT_NAME},
# Since this is the top level CMakeLists.txt, the project name is also accessible
# with ${CMAKE_PROJECT_NAME} (both CMake variables are in-sync within the top level
# build script scope).
project("h26x")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
#
# In this top level CMakeLists.txt, ${CMAKE_PROJECT_NAME} is used to define
# the target library name; in the sub-module's CMakeLists.txt, ${PROJECT_NAME}
# is preferred for the same purpose.
#
# In order to load a library into your app from Java/Kotlin, you must call
# System.loadLibrary() and pass the name of the library defined here;
# for GameActivity/NativeActivity derived applications, the same library name must be
# used in the AndroidManifest.xml file.
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
add_library(${CMAKE_PROJECT_NAME} SHARED
        # List C/C++ source files with relative paths to this CMakeLists.txt.
        h26x/BufferStream.cpp
        h26x/NalByteStream.cpp
        h26x/RWBufferStream.cpp
        h26x/RWNalByteStream.cpp
        h26x/BitStream.cpp
        h26x/RWBitStream.cpp
        h26x/ExpGolomb.cpp
        h26x/SPS.cpp
        h26x/PPS.cpp
        h26x/VuiParameters.cpp
        h26x/KMPSearch.cpp
        h26x/NalUnit.cpp
        h26x/NalUnitFinder.cpp
        h26x/PictureCounter.cpp
        h26x/SliceHeader.cpp
)
target_include_directories(h26x
        PUBLIC
        h26x/include/
)
# Specifies libraries CMake should link to your target library. You
# can link libraries from various origins, such as libraries defined in this
# build script, prebuilt third-party libraries, or Android system libraries.
target_link_libraries(${CMAKE_PROJECT_NAME}
)
set(GOOGLETEST_ROOT ${ANDROID_NDK}/sources/third_party/googletest)
add_library(gtest STATIC ${GOOGLETEST_ROOT}/src/gtest_main.cc ${GOOGLETEST_ROOT}/src/gtest-all.cc)
target_include_directories(gtest PRIVATE ${GOOGLETEST_ROOT})
target_include_directories(gtest PUBLIC ${GOOGLETEST_ROOT}/include)

add_executable(h26xtest
        h26x/test/BitReaderTest.cpp
        h26x/test/BitWriterTest.cpp
        h26x/test/SpsTest.cpp
        h26x/test/PpsTest.cpp
#        h26x/test/NalUnitFinderTest.cpp
#        h26x/test/SliceHeaderTest.cpp
)
target_link_libraries(h26xtest h26x gtest)