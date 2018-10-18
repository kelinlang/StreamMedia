package com.stream.media.demo.business;

import android.content.Context;
import android.os.Handler;
import android.os.HandlerThread;
import android.view.Surface;

import com.stream.media.demo.camera.Camera2Holder;
import com.stream.media.demo.utils.CommonLib;


/**
 * Created by dengjun on 2018/10/16.
 */

public class VideoSeviceTest {
    private Camera2Holder mCamera2Holder;

    private HandlerThread mHandlerThread;
    private Handler mBackgroundHandler;

    public void init(Context applicationContext){
        CommonLib.getInstance().setContext(applicationContext);
        startBackgroundThread();
        if (mCamera2Holder == null){
            mCamera2Holder = new Camera2Holder(CommonLib.getInstance().getContext(),mBackgroundHandler);
        }
    }

    public void release(){
        closeCamera();
        stopBackgroundThread();
    }

    public void openCamera(){
        if (mBackgroundHandler != null){
            mBackgroundHandler.post(new Runnable() {
                @Override
                public void run() {
                    mCamera2Holder.openCamera();
                }
            });
        }
    }

    public void closeCamera(){
        if (mBackgroundHandler != null){
            mBackgroundHandler.post(new Runnable() {
                @Override
                public void run() {
                    if (mCamera2Holder != null){
                        mCamera2Holder.closeCamera();
                    }
                }
            });
        }
    }

    public void setPreviewSurface(Surface  previewSurface){
        if (mCamera2Holder != null){
            mCamera2Holder.setPerviewSurface(previewSurface);
        }
    }

    public void startPreview(){
        if (mBackgroundHandler != null){
            mBackgroundHandler.post(new Runnable() {
                @Override
                public void run() {
                    if (mCamera2Holder != null){
                        mCamera2Holder.startPreview();
                    }
                }
            });
        }
    }

    /**
     * Starts a background thread and its {@link Handler}.
     */
    private void startBackgroundThread() {
        mHandlerThread = new HandlerThread("CameraBackground");
        mHandlerThread.start();
        mBackgroundHandler = new Handler(mHandlerThread.getLooper());
    }

    /**
     * Stops the background thread and its {@link Handler}.
     */
    private void stopBackgroundThread() {
        if (mHandlerThread != null){
            mHandlerThread.quitSafely();
            try {
                mHandlerThread.join();
                mHandlerThread = null;
                mBackgroundHandler = null;
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}
