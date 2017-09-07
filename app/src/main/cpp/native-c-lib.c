//
// Created by lijian on 2017/9/6.
//
#include <jni.h>

JNIEXPORT jstring JNICALL
Java_com_lijian_learnjni_MainActivity_stringFromC_1JNI(JNIEnv *env, jobject instance) {
    char *returnValue = "Hello from C";
    return (*env)->NewStringUTF(env, returnValue);
}

