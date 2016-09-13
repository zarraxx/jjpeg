//
// Created by 夏申频 on 16/9/12.
//

#ifndef GDCMJPEGTEST_JINPUTSTREAM_H
#define GDCMJPEGTEST_JINPUTSTREAM_H
#include <jni/jni.hpp>
struct InputStream;
struct OutputStream;

class JInputStream {
private:
    jni::Object<InputStream>& jin;
    jni::JNIEnv& jenv;
public:
    typedef jni::Array<jni::jbyte> JByteArray;
    JInputStream(jni::Object<InputStream>& in,jni::JNIEnv& env);
    jint read();
    jint read(std::vector<jni::jbyte>& buffer,jint off,jint len);
    jint read(std::vector<jni::jbyte>& buffer);
    jlong skip(jlong n);
    int available();
    void close();
    void mark(jint readlimit);
    void reset();
    jboolean markSupported();
};


#endif //GDCMJPEGTEST_JINPUTSTREAM_H
