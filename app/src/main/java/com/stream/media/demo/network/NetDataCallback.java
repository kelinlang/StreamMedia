package com.stream.media.demo.network;

/**
 * Created by dengjun on 2019/1/24.
 */

public interface NetDataCallback {
    void onData(byte[] data,int pos,int len);
}
