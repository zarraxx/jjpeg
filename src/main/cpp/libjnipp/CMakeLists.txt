cmake_minimum_required(VERSION 3.6)
project(libjnipp)

find_package(Java REQUIRED)
include(UseJava)

set(CMAKE_JAVA_COMPILE_FLAGS "-source" "1.7" "-target" "1.7")


if (NOT JAR_OUTPUT_PATH)
    set(JAR_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
endif ()

add_subdirectory(java)
add_subdirectory(cpp)