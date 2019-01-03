package com.stream.media.demo;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.medialib.audio.base.OpenslEs;
import com.medialib.video.OpenGlEs;
import com.stream.media.demo.business.VideoSeviceTest;
import com.stream.media.demo.utils.MLog;
import com.stream.media.demo.utils.PermssionUtils;


public class TestCameraActivity extends AppCompatActivity implements SurfaceHolder.Callback{
    private static final int RESULT_CODE_CAMERA = 105;
    private SurfaceView mCameraPreviewView;
    private SurfaceView mCameraPreviewViewJni;

    private VideoSeviceTest mVideoSeviceTest;

    public static OpenGlEs openGlEs = new OpenGlEs();
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_test_camera);
        mCameraPreviewView = (SurfaceView)findViewById(R.id.camera_preview_sv);
        mCameraPreviewView.getHolder().addCallback(this);
        mCameraPreviewViewJni = (SurfaceView)findViewById(R.id.camera_preview_jni_sv);
        mCameraPreviewViewJni.getHolder().addCallback(jniCallback);

        mVideoSeviceTest = new VideoSeviceTest();
        mVideoSeviceTest.init(this.getApplicationContext());

    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == RESULT_CODE_CAMERA && grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
            mVideoSeviceTest.openCamera();
//            mVideoSeviceTest.startPreview();
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        MLog.i("onDestroy");
        mVideoSeviceTest.release();
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        mVideoSeviceTest.setPreviewSurface(holder.getSurface());


        if (PermssionUtils.checkPermision(this, Manifest.permission.CAMERA)){
            mVideoSeviceTest.openCamera();
            mVideoSeviceTest.startPreview();
        }else {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.CAMERA}, RESULT_CODE_CAMERA);
        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        MLog.i("surfaceChanged");
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        MLog.i("surfaceDestroyed");
        mVideoSeviceTest.closeCamera();

    }


    SurfaceHolder.Callback jniCallback = new SurfaceHolder.Callback() {
        @Override
        public void surfaceCreated(SurfaceHolder holder) {
            openGlEs.setSurface(holder.getSurface());
            openGlEs.start();
        }

        @Override
        public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
            MLog.d("surfaceChanged weight : "+width+ " ,height : "+ height);
        }

        @Override
        public void surfaceDestroyed(SurfaceHolder holder) {
            openGlEs.stop();
        }
    };
}
