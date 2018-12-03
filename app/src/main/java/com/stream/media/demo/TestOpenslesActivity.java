package com.stream.media.demo;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import com.medialib.audio.base.OpenslEs;

public class TestOpenslesActivity extends AppCompatActivity {

    private OpenslEs openslEs = new OpenslEs();
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_test_opensles);
        openslEs.process_opensles_playstart();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        openslEs.process_opensles_playstop();
    }
}
