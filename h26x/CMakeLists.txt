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
enable_testing()
add_subdirectory(test)