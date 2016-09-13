find_package(JNI)

#set(BUILD_SHARED_LIBS ON)

if (JNI_FOUND)
    message (STATUS "JNI_INCLUDE_DIRS=${JNI_INCLUDE_DIRS}")
    message (STATUS "JNI_LIBRARIES=${JNI_LIBRARIES}")
endif()

message(STATUS "jni include:" ${JNI_INCLUDE_DIRS})
include_directories(${JNI_INCLUDE_DIRS} ${JAR_OUTPUT_PATH}  ${CMAKE_CURRENT_SOURCE_DIR})
link_directories(${JNI_LIBRARIES} )

set(CPP_SRC JNISample.cpp JInputStream.cpp JInputStream.h)


add_library(jnicpp SHARED ${CPP_SRC} )

set_target_properties(jnicpp PROPERTIES OUTPUT_NAME "jnipp")