package com.medialib.audio.base;

import com.stream.media.demo.utils.MLog;

import java.util.Arrays;

/**
 * Created by dengjun on 2018/12/2.
 */

public class OpenslEs {
    public native int  process_opensles_playstart();
    public native int  process_opensles_playstop();

    public native void   process_opensles_putplaydata(short[] pcmData,int pcmDataSize);

    public static short[] process_opensles_getPlayData(){
        return new short[160];
    }

    public static int process_opensles_getPlayData(short[] audioData){
        Arrays.fill(audioData,(short) 0);
        return 1600;
    }

    public static short[]  process_opensles_getPlayShortData(){
        return new short[1600];
    }

    public static int process_opensles_getPlayDataBytes(int dataLen,short[] audioData){
        Arrays.fill(audioData,(short) 0);
        return 1600;
    }

    public static byte[] getPlayDataBytes(){
        return new byte[3200];
    }

    public void printMessage(int message){
        MLog.i("printMessage: " + message);
    }

    public static int printLog(String log){
        MLog.i("printLog: " + log);
        return 12;
    }

    public static String getString(){
        MLog.i("printLog: getSting" );
        return "----getSting------";
    }
}
