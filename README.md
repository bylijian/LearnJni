#前言
感觉现在的 Android 开发都进入深水区了，单纯会个Java，写个Xml布局越来越难找工作了。JNI学习看起来是必须的了。
注意：
本文侧重编程实践和代码讲解，强烈建议阅读理论知识《Android 设计与实现 》第二章作为准备。
 
#练习1：Hello-Jni
演示通过Java 通过Jni调用Native代码，然后报调用的结果显示在TextView上

基本流程如下：

 

在Android studio  2.3 版本，新建项目的时候，勾选C++ support，系统创建的工程基本就是Hello-Jni
下面简单讲一下 ，基于 Cmake 编译的支持 Native 代码的工程 与普通的工程区别：

## app 目录下的**build.gradle**

```
apply plugin: 'com.android.application'

android {
    compileSdkVersion 26
    buildToolsVersion "26.0.1"
    defaultConfig {
        applicationId "com.lijian.learnjni"
        minSdkVersion 21
        targetSdkVersion 26
        versionCode 1
        versionName "1.0"
        testInstrumentationRunner "android.support.test.runner.AndroidJUnitRunner"
        //和普通的Android 工程不同，为了支持编译C,C++ 代码，需要 加入 externalNativeBuild
        externalNativeBuild {
            cmake {
                cppFlags "-std=c++11 -frtti -fexceptions" //指定编译的c++ 使用的是 c++11，后面的是编译参数
            }
        }
    }
    buildTypes {
        release {
            minifyEnabled false
            proguardFiles getDefaultProguardFile('proguard-android.txt'), 'proguard-rules.pro'
        }
    }
    externalNativeBuild {
        cmake {
            path "CMakeLists.txt"//这个指定了Cmake使用的编译脚本CMakeLists.txt的路径，和build.gradle 同级
        }
    }
}

dependencies {
    compile fileTree(dir: 'libs', include: ['*.jar'])
    androidTestCompile('com.android.support.test.espresso:espresso-core:2.2.2', {
        exclude group: 'com.android.support', module: 'support-annotations'
    })
    compile 'com.android.support:appcompat-v7:26.+'
    compile 'com.android.support.constraint:constraint-layout:1.0.2'
    testCompile 'junit:junit:4.12'
}

```

## Cmake 编译脚本 CMakeLists.txt
Cmake 概念详情自行百度，简单来说：
CMake是一个跨平台的安装（编译）工具，可以用简单的语句来描述所有平台的安装(编译过程)

新版本的 Android Studio支持 开发者自己编写CMake脚本，然后的build Apk的时候，自动解析CMake脚本调用 NDK工具链，编译原生的C，C++代码生成
Android 系统支持的 *.so 文件 

```
# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

#指定Cmake最低版本

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

#添加一个名字为 native-lib 的 SHARED library ，它最终的 文件名是 libnative-lib.so
#源文件是 src/main/cpp/native-lib.cpp

add_library( # Sets the name of the library.
            native-lib

            # Sets the library as a shared library.
            SHARED

            # Provides a relative path to your source file(s).
            src/main/cpp/native-lib.cpp )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

#Cmake 默认包含了系统的library比如log，使用log只需要找到它，
#并把它的path保存到一个变量即可

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

#指定 native-lib 需要链接的library

target_link_libraries( # Specifies the target library.
                      native-lib

                      # Links the target library to the log library
                      # included in the NDK.
                      ${log-lib} )
```

## Java层 声明&&使用 Native 函数

```
public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_jni_c_plus_plus);
        // Java 层调用 Jni 方法就像调 Java 方法一样
        tv.setText(stringFromJNI());

        TextView tv1 = (TextView) findViewById(R.id.sample_jni_c);
        tv1.setText(stringFromC_JNI());
    }

    /**
    * 声明函数是由 native 代码实现的，只需要加一个 native 关键字即可
    */
    public native String stringFromJNI();
    
    public native String stringFromC_JNI();

    // 使用静态代码块保证类加载的时候，就加载 native 的so库
    static {
        System.loadLibrary("native-lib");
        System.loadLibrary("native-c-lib");
    }
}

```


##编写 C++代码实现 Hello from C++
```
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


```

##编写 C代码实现 Hello from C
```
#include <jni.h>

JNIEXPORT jstring JNICALL
Java_com_lijian_learnjni_MainActivity_stringFromC_1JNI(JNIEnv *env, jobject instance) {
    char *returnValue = "Hello from C";
    return (*env)->NewStringUTF(env, returnValue);
}

```

##比较 JNI 使用 C 语言编写 和使用 C++ 编写的差异
* extern "C" 引发的血案！
我尝试在 C++的JNI代码中去掉 extern "C" ,然后应用就奔溃了
愿意是Java层找不到native的调用入口，为什么？
因为C++支持函数的重载，为了区分重载函数，编译的C++代码的函数名称并不是原来的名称，而是带有函数参数的名称
Jni调用本身就是通过名称找到调用入口的，函数名称被改变了，自然就找不到了
详情参考：
http://www.jianshu.com/p/5d2eeeb93590

**extern "C"**的作用是告诉编译这段代码安装 C 的编译方式编译，自然就保留了原始的函数名称，不会出现调用入口找不到本科的问题

* env  是个什么东西？
env 可以说是JNI的大总管，JNI代码中，需要用到跟java层有关的的方法基本都在里面
在C++中
env的使用是这样的：
env->NewStringUTF(hello.c_str());
在C中是这样的：
(*env)->NewStringUTF(env, returnValue);
总之，就是因为在C++，C中 env的定义不一样造成了
具体参考 《Android的设计与实现 第二章》

#练习二：Hello-Jni-Callback
这个练习是学习，如何在JNI代码中调用Java的静态函数，public 成员函数，private成员函数。代码比较简单，一些细节都有在代码注释。

##Nativie 调用 Java的static方法：


###Java代码 ：
非常简单，就是实现一个static方法即可
```
public class JavaForNative {

    private static final String TAG = "JavaForNative";

    //Java负责实现一个static方法
    public static String getBuildVersion() {
        return Build.VERSION.RELEASE;
    }

   //省略代码
}

```
 

###Native 代码：

####找到Class实例
```

    //通过 (*env)->FindClass() 找到JavaForNative类的Class实例
    jclass clz = (*env)->FindClass(env, "com/lijian/learnjni/JavaForNative");

    //这个实例的内存由Java虚拟机管理，使用(*env)->NewGlobalRef()创建一个全局的引用，
    // 这样，会增加引用计数，除非解除引用，否则Java虚拟机不会回收这个内存
    javaContext.javaForNativeClz = (*env)->NewGlobalRef(env, clz);

```
####获取static方法的ID，并调用
```
  //调用Java的static方法
    jmethodID staticMethodID = (*env)->GetStaticMethodID(env, javaContext.javaForNativeClz,
                                                        "getBuildVersion", "()Ljava/lang/String;");

    jstring buildVersion = (*env)->CallStaticObjectMethod(env, javaContext.javaForNativeClz,
                                                          staticMethodID);

    const char *version = (*env)->GetStringUTFChars(env, buildVersion, NULL);

```
##Native调用Java的成员方法：
其中，调用public的成员方法和private的成员方法是一样的，下面只颜色了调用public成员方法的代码：

###找到Class实例，并通过调用这个类的构造方法，创建这个类的对象
```
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

```


###获取成员方法的ID，并调用
```
  //调用Java的 public 成员方法
    jmethodID publicMethodID = (*env)->GetMethodID(env, javaContext.javaForNativeClz,
                                                  "getTotalMemory", "()J");

    jlong size = (*env)->CallLongMethod(env, javaContext.javaForNativeObj, publicMethodID);


```


#练习 3：Hello-lib
这个练习演示了如何在NDK工程中使用已经编译好的 .a .so 
分为两大部分

##使用Cmake编译生成 .a .so文件


#使用 .a ,.so文件

注意 Android的App中如果在Native中使用到了.a文件，会和调用这个.a库的代码一起编译为一个so文件















