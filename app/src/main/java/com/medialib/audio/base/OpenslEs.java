package com.medialib.audio.base;

import java.util.Arrays;

/**
 * Created by dengjun on 2018/12/2.
 */

public class OpenslEs {
    public native int  process_opensles_playstart();
    public native int  process_opensles_playstop();

    public static short[] process_opensles_getPlayData(){
        return new short[160];
    }

    public static int process_opensles_getPlayDataBytes(int dataLen,byte[] audioData){
        Arrays.fill(audioData,(byte) 0);
        return 3200;
    }
}
