package com.lijian.learnjni;

import android.os.Build;
import android.text.TextUtils;
import android.util.Log;

/**
 * Created by lijian on 2017/9/7.
 */

public class JavaForNative {

    private static final String TAG = "JavaForNative";

    static {
    }

    /**
     * Java负责实现一个static方法getBuildVersion()
     */
    public static String getBuildVersion() {
        return Build.VERSION.RELEASE;
    }

    /**
     * Java负责实现一个public getTotalMemory()
     */
    public long getTotalMemory() {
        return Runtime.getRuntime().totalMemory();
    }

    /**
     * Java负责实现一个private getTotalMemory()
     */
    private void sendMessage(String msg) {
        if (TextUtils.isEmpty(msg)) {
            Log.e(TAG, "msg is empty");
            return;
        }
        if (msg.toLowerCase().contains("error")) {
            Log.e(TAG, "error msg=" + msg);
        } else {
            Log.d(TAG, "debug msg=" + msg);
        }
    }

}
