cmake_minimum_required(VERSION 3.6)
project(gdcmJpegTest)


if(NOT LIBRARY_OUTPUT_PATH)
    set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin CACHE PATH "Single output directory for building all libraries.")
    mark_as_advanced(LIBRARY_OUTPUT_PATH)
endif()

if (NOT JAR_OUTPUT_PATH)
    set(JAR_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH})
endif ()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")




add_subdirectory(libjpeg libjpeg)

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/jplossless.raw" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

#add_subdirectory(libjnipp)

#add_subdirectory(src)