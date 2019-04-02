package com.stream.media.jni;

/**
 * Created by dengjun on 2019/3/13.
 */

public class VideoData {
    String id;

    int dataFormat;

    int width;
    int height;

    int frameType;
    long timeStamp;

    int videoDataLen;
    byte[] videoData;



    byte[] sps;
    int spsLen;
    byte[] pps;
    int ppsLen;

}
