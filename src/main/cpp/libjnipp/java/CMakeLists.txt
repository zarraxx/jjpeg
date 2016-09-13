set(JAVA_SRC JNISample.java JNITest.java)

set(CMAKE_JAR_CLASSES_PREFIX cn/net/xyan/jnipp)
add_jar(jnipp ${JAVA_SRC} OUTPUT_DIR ${JAR_OUTPUT_PATH} ENTRY_POINT cn/net/xyan/jnipp/JNITest)

Create_javah(
        TARGET JNISample.h
        #GENERATED_FILES files_headers
        CLASSES cn.net.xyan.jnipp.JNISample
        CLASSPATH jnipp
        OUTPUT_NAME ${JAR_OUTPUT_PATH}/JNISample.h
        #OUTPUT_DIR ${JAR_OUTPUT_PATH}
        )

