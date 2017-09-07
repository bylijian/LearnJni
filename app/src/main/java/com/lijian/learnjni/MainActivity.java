package com.lijian.learnjni;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

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
