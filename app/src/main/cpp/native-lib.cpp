#include <jni.h>
#include <string>

//注意这里之所以要extern "C" 因为 强制 C++使用C的方式编译，保持函数名称，这样编译的函数名称才能和 Java层名称对应上
extern "C"

JNIEXPORT jstring JNICALL
Java_com_lijian_learnjni_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
