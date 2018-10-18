package com.stream.media.demo.nativelib;

import android.view.Surface;

import com.stream.media.demo.camera.VideoDataInfo;

/**
 * Created by dengjun on 2018/10/18.
 */

public class StreamMediaNative {
    public static native void setSurface(Surface surface);
    public static native void init();
    public static native void release();
    public static native void pushVideoData(byte[] videoData, VideoDataInfo videoDataInfo);
}
