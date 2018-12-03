package com.stream.media.demo;

import android.Manifest;
import android.content.Intent;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import com.stream.media.demo.utils.CheckPermissionUtils;

public class MainActivity extends AppCompatActivity {
    private static final int RESULT_CODE_WRITE_EXTERNAL_SCARD = 102;
    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        if (CheckPermissionUtils.isNeedAddPermission(MainActivity.this, Manifest.permission.WRITE_EXTERNAL_STORAGE)) {
            ActivityCompat.requestPermissions(MainActivity.this, new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, RESULT_CODE_WRITE_EXTERNAL_SCARD);
        } else {

        }

        // Example of a call to a native method
//        onToTestCameraView(null);
    }


    public void onToTestCameraView(View view){
        startActivity(new Intent(this,TestCameraActivity.class));
    }

    public void onToTestOpenSLES(View view){
        startActivity(new Intent(this,TestOpenslesActivity.class));
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
}
