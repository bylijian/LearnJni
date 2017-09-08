//
// Created by lijian on 2017/9/7.
//
#include <jni.h>
#include <android/log.h>
#include <sys/types.h>
#include <string.h>

// Android log function wrappers
static const char *kTAG = "native-java-callback";
#define LOGI(...) \
  ((void)__android_log_print(ANDROID_LOG_INFO, kTAG, __VA_ARGS__))
#define LOGW(...) \
  ((void)__android_log_print(ANDROID_LOG_WARN, kTAG, __VA_ARGS__))
#define LOGE(...) \
  ((void)__android_log_print(ANDROID_LOG_ERROR, kTAG, __VA_ARGS__))

typedef struct java_context {
    JavaVM *javaVM;
    jclass javaForNativeClz;
    jobject javaForNativeObj;
    jclass mainActivityClz;
    jobject mainActivityObj;
    pthread_mutex_t lock;
    int done;
} JavaContext;

JavaContext javaContext;


void callJavaForNative(JNIEnv *pInterface, jobject pVoid);

void sendMsgToJava(JNIEnv *env, const char *msg);

JNIEXPORT void sendMsgToJava(JNIEnv *env, const char *msg) {
    LOGI("sendMsgToJava()");
    jstring javaMsg = (*env)->NewStringUTF(env, msg);
    jmethodID privateMethod = (*env)->GetMethodID(env, javaContext.javaForNativeClz, "sendMessage",
                                                  "(Ljava/lang/String;)V");
    (*env)->CallVoidMethod(env, javaContext.javaForNativeObj, privateMethod, javaMsg);

    //对应Jni层的java局部引用，在使用结束后必须手动移除引用，防止内存泄漏
    (*env)->DeleteLocalRef(env, javaMsg);
}

void callJavaForNative(JNIEnv *env, jobject jobj) {
    if (env == NULL || jobj == NULL) {
        return;
    }
    //调用Java的static方法
    jmethodID staticMethodID = (*env)->GetStaticMethodID(env, javaContext.javaForNativeClz,
                                                         "getBuildVersion", "()Ljava/lang/String;");

    jstring buildVersion = (*env)->CallStaticObjectMethod(env, javaContext.javaForNativeClz,
                                                          staticMethodID);

    const char *version = (*env)->GetStringUTFChars(env, buildVersion, NULL);
    if (!version) {
        LOGE("getBuildVersion failed");
        return;
    }
    LOGI("get BuildeVersion - %s", version);

    //调用Java的 public 成员方法
    jmethodID publicMethodID = (*env)->GetMethodID(env, javaContext.javaForNativeClz,
                                                   "getTotalMemory", "()J");

    jlong size = (*env)->CallLongMethod(env, javaContext.javaForNativeObj, publicMethodID);

    //编写一个函数调用Java的 public 成员方法
    sendMsgToJava(env, "send msg to Java from java!");
}

/**
 * JNI_OnLoad(JavaVM *vm, void *reserved) 在Java层调用System.loadLibrary()
 * 然后系统加载so库的时候被调用，仅调用一次
 * @param vm
 * @param reserved
 * @return
 */
jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    LOGI("JNI_OnLoad()");
    JNIEnv *env;
    //判断当前系统是否支持 1.6 版本的JNI
    if ((*vm)->GetEnv(vm, (void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR; // JNI version not supported.
    }
    //初始化javaContext的内存
    memset(&javaContext, 0, sizeof(javaContext));

    //保存JavaVM指针到javaContext
    javaContext.javaVM = vm;

    //通过 (*env)->FindClass() 找到JavaForNative类的Class实例
    jclass clz = (*env)->FindClass(env, "com/lijian/learnjni/JavaForNative");

    //这个实例的内存由Java虚拟机管理，使用(*env)->NewGlobalRef()创建一个全局的引用，
    // 这样，会增加引用计数，除非解除引用，否则Java虚拟机不会回收这个内存
    javaContext.javaForNativeClz = (*env)->NewGlobalRef(env, clz);

    //通过Class实例找到 JavaForNative 构造函数（无参）的MethodID
    jmethodID javaForNativeCtor = (*env)->GetMethodID(env, javaContext.javaForNativeClz, "<init>",
                                                      "()V");
    //通过MethodID调用构造方法创建一个JavaForNative实例
    jobject javaForNativeObj = (*env)->NewObject(env, javaContext.javaForNativeClz,
                                                 javaForNativeCtor);
    javaContext.javaForNativeObj = (*env)->NewGlobalRef(env, javaForNativeObj);

    callJavaForNative(env, javaContext.javaForNativeObj);

    //支持的话，返回当前的JNI版本
    return JNI_VERSION_1_6;
}