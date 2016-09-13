//
// Created by 夏申频 on 16/9/12.
//

#include "JInputStream.h"

struct InputStream  { static constexpr auto Name() { return "java/io/InputStream"; } };

JInputStream::JInputStream(jni::Object<InputStream> &in,jni::JNIEnv& env):jin(in),jenv(env) {

}

jint JInputStream::read() {
    /*static*/ auto inputStream = *jni::Class<InputStream>::Find(jenv).NewGlobalRef(jenv).release();
    /*static*/ auto javaRead = inputStream.GetMethod<jni::jint()>(jenv,"read");
    jint r = jin.Call(jenv,javaRead);
    return  r;
}

jint JInputStream::read(std::vector<jni::jbyte> &buffer) {
    /*static*/ auto inputStream = *jni::Class<InputStream>::Find(jenv).NewGlobalRef(jenv).release();
    /*static*/ auto javaRead = inputStream.GetMethod<jni::jint(JByteArray)>(jenv,"read");

    JByteArray jBuffer = jni::Make<JByteArray>(jenv,buffer);

    jint r = jin.Call(jenv,javaRead,jBuffer);

    //buffer.clear();
    std::vector<jni::jbyte> tmp = jni::Make<std::vector<jni::jbyte>>(jenv,jBuffer);

    buffer.swap(tmp);

    return  r;
}

jint JInputStream::read(std::vector<jni::jbyte> &buffer, jint off, jint len) {

    /*static*/ auto inputStream = *jni::Class<InputStream>::Find(jenv).NewGlobalRef(jenv).release();
    /*static*/ auto javaRead = inputStream.GetMethod<jni::jint(JByteArray,jni::jint,jni::jint)>(jenv,"read");

    JByteArray jBuffer = jni::Make<JByteArray>(jenv,buffer);

    jint r = jin.Call(jenv,javaRead,jBuffer,off,len);

    std::vector<jni::jbyte> tmp = jni::Make<std::vector<jni::jbyte>>(jenv,jBuffer);

    buffer.swap(tmp);

    return  r;
}

jlong JInputStream::skip(jlong n) {
    /*static*/ auto inputStream = *jni::Class<InputStream>::Find(jenv).NewGlobalRef(jenv).release();
    /*static*/ auto m = inputStream.GetMethod<jni::jlong(jni::jlong)>(jenv,"skip");

    return  jin.Call(jenv,m,n);
}

jint JInputStream::available() {
    /*static*/ auto inputStream = *jni::Class<InputStream>::Find(jenv).NewGlobalRef(jenv).release();
    /*static*/ auto m = inputStream.GetMethod<jni::jint()>(jenv,"available");

    return  jin.Call(jenv,m);
}

void JInputStream::close() {
    /*static*/ auto inputStream = *jni::Class<InputStream>::Find(jenv).NewGlobalRef(jenv).release();
    /*static*/ auto m = inputStream.GetMethod<void()>(jenv,"close");
    jin.Call(jenv,m);
}

void JInputStream::mark(jint readlimit){
    /*static*/ auto inputStream = *jni::Class<InputStream>::Find(jenv).NewGlobalRef(jenv).release();
    /*static*/ auto m = inputStream.GetMethod<void(jint)>(jenv,"mark");
    jin.Call(jenv,m,readlimit);
}
void JInputStream::reset(){
    /*static*/ auto inputStream = *jni::Class<InputStream>::Find(jenv).NewGlobalRef(jenv).release();
    /*static*/ auto m = inputStream.GetMethod<void()>(jenv,"reset");
    jin.Call(jenv,m);
}
jboolean JInputStream::markSupported(){
    /*static*/ auto inputStream = *jni::Class<InputStream>::Find(jenv).NewGlobalRef(jenv).release();
    /*static*/ auto m = inputStream.GetMethod<jboolean()>(jenv,"markSupported");
    return jin.Call(jenv,m);
}