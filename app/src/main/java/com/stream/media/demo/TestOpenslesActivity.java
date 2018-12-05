package com.stream.media.demo;

import android.os.Process;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import com.medialib.audio.base.OpenslEs;
import com.stream.media.demo.utils.MLog;

public class TestOpenslesActivity extends AppCompatActivity {

    private OpenslEs codecJni = new OpenslEs();

    private Thread recordThread;
    protected volatile boolean runFlag = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_test_opensles);
//        codecJni.process_opensles_playstart();


        start();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
//        codecJni.process_opensles_playstop();

        stop();
    }


    private void start(){
        runFlag = true;
        recordThread = new Thread(new Runnable() {
            @Override
            public void run() {
                Process.setThreadPriority(Process.THREAD_PRIORITY_URGENT_AUDIO);
                MLog.d("AudioRecorderHandler start run() tid=" + Thread.currentThread().getId());


                int retVal = codecJni.process_opensles_playstart();
                MLog.i("record process_opensles_recordstart");
                if (retVal == 0){
                    while (runFlag){
                        codecJni.process_opensles_putplaydata(new short[1600],1600);

                        try {
                            Thread.sleep(40);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                    }
                    codecJni.process_opensles_playstop();

                }else {
                    MLog.e("process_opensles_recordstart return value : "+retVal);
                    codecJni.process_opensles_playstop();


                }
            }
        });
        recordThread.start();
    }



    private void stop(){
        runFlag = false;
        if(recordThread != null){
            try {
                recordThread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}
