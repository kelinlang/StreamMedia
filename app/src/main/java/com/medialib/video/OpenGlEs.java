package com.medialib.video;

import android.view.Surface;

/**
 * Created by dengjun on 2018/12/25.
 */

public class OpenGlEs {
    public native int setSurface(Surface surface);
    public native int setMatrix(float[] matrix);
    public native int start();
    public native int stop();
    public native void sendYuvData(int yuvType,byte[] yuvData,int yuvDataLen);
}
