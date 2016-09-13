include_directories(${PROJECT_BINARY_DIR})

set(SOURCE_FILES main.cpp jpegCodec.cpp jpegCodec.h )
add_executable(gdcmJpegTest ${SOURCE_FILES})

target_link_libraries(gdcmJpegTest jpeg16 jpeg12 jpeg8)