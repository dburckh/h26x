add_library(${CMAKE_PROJECT_NAME} SHARED
        # List C/C++ source files with relative paths to this CMakeLists.txt.
        BufferStream.cpp
        NalByteStream.cpp
        BitStream.cpp
        ExpGolomb.cpp
        SPS.cpp
        PPS.cpp
        VuiParameters.cpp
        KMPSearch.cpp
        NalUnit.cpp
        NalUnitFinder.cpp
        PictureCounter.cpp
        SliceHeader.cpp
)
target_include_directories(h26x
        PUBLIC
        include/
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
        test/BitReaderTest.cpp
        test/BitWriterTest.cpp
        test/SpsTest.cpp
        test/PpsTest.cpp
        #        h26x/test/NalUnitFinderTest.cpp
        test/SliceHeaderTest.cpp
)
target_link_libraries(h26xtest h26x gtest)