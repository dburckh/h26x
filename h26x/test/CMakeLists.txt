set(GOOGLETEST_ROOT ${ANDROID_NDK}/sources/third_party/googletest)
add_library(gtest STATIC ${GOOGLETEST_ROOT}/src/gtest_main.cc ${GOOGLETEST_ROOT}/src/gtest-all.cc)
target_include_directories(gtest PRIVATE ${GOOGLETEST_ROOT})
target_include_directories(gtest PUBLIC ${GOOGLETEST_ROOT}/include)

add_executable(h26xtest
        BitReaderTest.cpp
        BitWriterTest.cpp
        SpsTest.cpp
        PpsTest.cpp
        NalUnitFinderTest.cpp
        SliceHeaderTest.cpp
        PictureCountTest.cpp
)
target_link_libraries(h26xtest h26x gtest)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bbb.h264 ${CMAKE_CURRENT_BINARY_DIR}/Debug/bbb.h264 COPYONLY)
if (WIN32)
    add_custom_command(TARGET h26xtest POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:h26xtest> $<TARGET_RUNTIME_DLLS:h26xtest>
            COMMAND_EXPAND_LISTS
    )
endif()