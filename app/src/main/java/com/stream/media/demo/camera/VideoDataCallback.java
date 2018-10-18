package com.stream.media.demo.camera;

/**
 * Created by dengjun on 2018/10/16.
 */

public interface VideoDataCallback {
    void onData(byte[] videoData, VideoDataInfo videoDataInfo);
}
