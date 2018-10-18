package com.stream.media.demo.utils;

import android.content.Context;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;

/**
 * Created by dengjun on 2017/12/27.
 */

public class CommonLib {
    private static CommonLib instance = null;

    private Context mContext;

    /*后台线程loop*/
    private static HandlerThread handlerThread;

    private static Handler callbackHandler = new Handler(Looper.getMainLooper());

    static {
        initHandlerThread();
    }

    private CommonLib() {

    }

    public static CommonLib getInstance(){
        if (instance == null){
            synchronized (CommonLib.class){
                if (instance == null){
                    instance = new CommonLib();
                }
            }
        }
        return instance;
    }

    public Context getContext() {
        return mContext;
    }

    public void setContext(Context context) {
        this.mContext = context;
    }

    private static void initHandlerThread() {
        if (handlerThread == null) {
            handlerThread = new HandlerThread("HeadlerThead");
            handlerThread.setPriority(Thread.MAX_PRIORITY);
            handlerThread.start();
        }
    }

    private void releaseHandlerThread() {
        if (handlerThread != null && handlerThread.isAlive()) {
            handlerThread.quit();
            handlerThread = null;
        }
    }

    /**
     * 后台线程looper
     * @return
     */
    public Looper getBackLooper(){
        return handlerThread.getLooper();
    }

    public static Handler getCallbackHandler() {
        return callbackHandler;
    }
}

