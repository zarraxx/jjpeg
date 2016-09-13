//
// Created by 夏申频 on 16/9/12.
//

#include <jni/jni.hpp>
#include "JInputStream.h"
struct JNISample    { static constexpr auto Name() { return "cn/net/xyan/jnipp/JNISample"; } };
struct InputStream  { static constexpr auto Name() { return "java/io/InputStream"; } };
struct OutputStream { static constexpr auto Name() { return "java/io/OutputStream"; } };




static void RegisterHighLevel(JavaVM* vm)
{


    jni::JNIEnv& env ( jni::GetEnv(*vm)) ;

     static auto jniSample = *jni::Class<JNISample>::Find(env).NewGlobalRef(env).release();
     static auto inputStream = *jni::Class<InputStream>::Find(env).NewGlobalRef(env).release();
     static auto outputStream = *jni::Class<OutputStream>::Find(env).NewGlobalRef(env).release();

    //static auto out = system.GetStaticField<jni::Object<PrintStream>>(env, "out");
    //static auto println = printStream.GetMethod<void (jni::String)>(env, "println");

    auto read = [] (jni::JNIEnv& env, jni::Object<JNISample> thiz,  jni::Object<InputStream> in ,jni::Object<OutputStream> out)
    {
        const int buffer_size = 1024;
        std::vector<jbyte> buffer(buffer_size);

        //jni::Make<jni::String>(env, u"Hello, ");

        JInputStream jis(in,env);

        //jint r = jis.read(buffer);
        jis.mark(10);
        for (int i = 0; i < 5; ++i) {
            int v = buffer[i];
            std::cout<<i<<" = "<<jis.read()<<std::endl;
        }
        jis.reset();

        jint r = jis.read(buffer);

        for (int i = 0; i < r; ++i) {
            int v = buffer[i];
            std::cout<<i<<" = "<<v<<std::endl;
        }
        //jni::Array<jni::jbyte> jBuffer = jni::Make<jni::Array<jni::jbyte>>(env,buffer);

        //static auto javaRead = inputStream.GetMethod<jni::jint(jni::Array<jni::jbyte>,jni::jint,jni::jint)>(env,"read");

        //::jint r = in.Call(env,javaRead,jBuffer,0,buffer_size);

        //return r;
        //system.Get(env, out).Call(env, println,
         //                         jni::Make<jni::String>(env,
          //                                               u"Hello, " + jni::Make<std::u16string>(env, args.Get(env, 0)) + u" (Native High-Level)"));
    };
    auto f = jni::MakeNativeMethod("read", read);
    jni::RegisterNatives(env, jni::Class<JNISample>::Find(env), f);
}

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*)
{
    RegisterHighLevel(vm);
    return jni::Unwrap(jni::jni_version_1_2);
}