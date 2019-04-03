package com.stream.media.view;

import android.content.Context;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.lib.commonlib.utils.MLog;
import com.stream.media.camera.Camera2Impl;
import com.stream.media.camera.CameraInterface;
import com.stream.media.camera.CameraParam;
import com.stream.media.camera.PreviewDataCallback;
import com.stream.media.codec.VideoEncoder;
import com.stream.media.codec.VideoParam;
import com.stream.media.jni.MediaDataCallback;

@RequiresApi(Build.VERSION_CODES.LOLLIPOP)
public class CameraPriviewView extends SurfaceView implements SurfaceHolder.Callback, PreviewDataCallback , MediaDataCallback {
    private CameraInterface camera;

    private VideoParam videoParam = new VideoParam();
    private VideoEncoder videoEncoder = new VideoEncoder();

    public CameraPriviewView(Context context) {
        super(context);
        init();
    }

    public CameraPriviewView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public CameraPriviewView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }


    @RequiresApi(Build.VERSION_CODES.LOLLIPOP)
    public CameraPriviewView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        init();
    }


    void init(){
        getHolder().addCallback(this);
        camera = new Camera2Impl(getContext());
        camera.setPreviewDataCallback(this);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        MLog.d("camera preview surfaceCreated width : "+ getWidth()+ " , height : "+ getHeight());
        videoParam.setWidth(getWidth());
        videoParam.setHeight(getHeight());

        videoEncoder.setVideoParam(videoParam);
        videoEncoder.setDataCallback(this);
        videoEncoder.start();


        CameraParam cameraParam = new CameraParam();
        cameraParam.setPriviewWidth(getWidth());
        cameraParam.setPriviewHeight(getHeight());
        camera.setParam(cameraParam);
        camera.setSurface(holder.getSurface());
        camera.open();
        camera.startPreview();

    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        MLog.i("camera preview  surfaceChanged width : "+ width+ " , height : "+ height);
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        MLog.i("camera preview  surfaceDestroyed");
        camera.stopPreview();
        camera.close();

        videoEncoder.stop();
        videoEncoder.setDataCallback(null);

        getHolder().removeCallback(this);
    }

    //摄像头预留数据回调
    @Override
    public void onData(int type, byte[] data) {
        videoEncoder.inputData(data);
    }

    //编码后回调h264数据
    @Override
    public void onData(int type, Object data) {

    }

    public void setMediaDataCallback(MediaDataCallback mediaDataCallback){
        videoEncoder.setDataCallback(mediaDataCallback);
    }


}
