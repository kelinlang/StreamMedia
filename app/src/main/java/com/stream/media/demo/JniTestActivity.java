package com.stream.media.demo;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;

import com.stream.media.demo.jni.JniTest;

public class JniTestActivity extends AppCompatActivity {
    private JniTest jniTest;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_jni_test);
        jniTest = new JniTest();
    }


    public void onStart(View view){
        jniTest.start();
    }

    public void onStop(View view){
        jniTest.stop();
    }
}
