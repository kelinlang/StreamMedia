package com.medialib.audio.base;

/**
 * Created by dengjun on 2018/12/2.
 */

public class OpenslEs {
    public native int  process_opensles_playstart();
    public native int  process_opensles_playstop();

    public static short[] process_opensles_getPlayData(){
        return new short[160];
    }
}
