package com.stream.media.demo.camera;

import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.ImageFormat;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.CaptureResult;
import android.hardware.camera2.TotalCaptureResult;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.media.Image;
import android.media.ImageReader;
import android.os.Build;
import android.os.Handler;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.util.Range;
import android.util.Size;
import android.view.Surface;


import com.stream.media.demo.TestCameraActivity;
import com.stream.media.demo.nativelib.StreamMediaNative;
import com.stream.media.demo.utils.MLog;

import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;

/**
 * Created by dengjun on 2018/10/16.
 */

public class Camera2Holder {

    private Context mContext;

    private Handler mBackgroudHandler;

    private Surface previewSurface;

    private String curCameraId;
    private CameraDevice mCameraDevice;

    private ImageReader mImageReader;

    /**
     * A {@link CameraCaptureSession } for camera preview.
     */
    private CameraCaptureSession mCaptureSession;
    private CaptureRequest.Builder mPreviewRequestBuilder;
    private CaptureRequest mPreviewRequest;

    public Camera2Holder(Context mContext, Handler backgroudHandler) {
        this.mContext = mContext;
        this.mBackgroudHandler = backgroudHandler;
    }

    /**
     * {@link CameraDevice.StateCallback} is called when {@link CameraDevice} changes its state.
     */
    private final CameraDevice.StateCallback mStateCallback = new CameraDevice.StateCallback() {

        @Override
        public void onOpened(@NonNull CameraDevice cameraDevice) {
            // This method is called when the camera is opened.  We start camera preview here.
            MLog.i("onOpened");
            mCameraDevice = cameraDevice;
//            createCameraPreviewSession();
        }

        @Override
        public void onDisconnected(@NonNull CameraDevice cameraDevice) {
            MLog.i("onDisconnected");
            cameraDevice.close();
            mCameraDevice = null;
        }

        @Override
        public void onError(@NonNull CameraDevice cameraDevice, int error) {
            MLog.i("onError");
            cameraDevice.close();
            mCameraDevice = null;
        }

    };


    public void setPerviewSurface(Surface surface) {
        previewSurface = surface;
    }

    private void createCameraPreviewSession(){
        if (previewSurface != null && mCameraDevice != null){
            try {
//                mPreviewRequestBuilder = mCameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
//                mPreviewRequestBuilder.addTarget(previewSurface);

                mPreviewRequestBuilder = mCameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
                mPreviewRequestBuilder.addTarget(previewSurface);
                mPreviewRequestBuilder.addTarget(mImageReader.getSurface());

                mCameraDevice.createCaptureSession(Arrays.asList(previewSurface,mImageReader.getSurface()), new CameraCaptureSession.StateCallback() {
                    @Override
                    public void onConfigured(@NonNull CameraCaptureSession session) {
                        MLog.i("onConfigured");
                        if (null == mCameraDevice) {
                            return;
                        }

                        // When the session is ready, we start displaying the preview.
                        mCaptureSession = session;

                        try {
                            // Auto focus should be continuous for camera preview.
                            mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AF_MODE, CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_PICTURE);
//                            mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AE_MODE, CaptureRequest.CONTROL_AE_MODE_OFF);

//                            mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AF_MODE, CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_VIDEO);
                            // Flash is automatically enabled when necessary.


                            // Finally, we start displaying the camera preview.
                            mPreviewRequest = mPreviewRequestBuilder.build();
                            mCaptureSession.setRepeatingRequest(mPreviewRequest, null, mBackgroudHandler);
                        } catch (CameraAccessException e) {
                            e.printStackTrace();
                            MLog.e("CameraAccessException error ",e);
                        }
                    }

                    @Override
                    public void onConfigureFailed(@NonNull CameraCaptureSession session) {
                        MLog.i("onConfigureFailed");
                    }
                },mBackgroudHandler);
            } catch (CameraAccessException e) {
                e.printStackTrace();
                MLog.e("CameraAccessException error ",e);
            }
        }
    }

    private CameraCaptureSession.CaptureCallback mCaptureCallback
            = new CameraCaptureSession.CaptureCallback() {
        @Override
        public void onCaptureProgressed(@NonNull CameraCaptureSession session, @NonNull CaptureRequest request, @NonNull CaptureResult partialResult) {
            super.onCaptureProgressed(session, request, partialResult);
            MLog.i("onCaptureProgressed");
        }

        @Override
        public void onCaptureCompleted(@NonNull CameraCaptureSession session, @NonNull CaptureRequest request, @NonNull TotalCaptureResult result) {
            super.onCaptureCompleted(session, request, result);
        }
    };

    public void openCamera() {
        MLog.i("openCamera");
        CameraManager cameraManager = (CameraManager) mContext.getSystemService(Context.CAMERA_SERVICE);
        try {

            String cameraId = cameraManager.getCameraIdList()[0];

            CameraCharacteristics cameraCharacteristics = cameraManager.getCameraCharacteristics(cameraId);
            curCameraId = cameraId;
            if (ActivityCompat.checkSelfPermission(mContext, Manifest.permission.CAMERA) == PackageManager.PERMISSION_GRANTED) {
                cameraManager.openCamera(cameraId, mStateCallback, mBackgroudHandler);

                createImageReader();
            }else {
                MLog.i("not had camera permmsion");
            }
        } catch (CameraAccessException e) {
            e.printStackTrace();
            MLog.e("CameraAccessException error ",e);
        }

//        initCodec();
//        StreamMediaNative.init();
    }

    public void closeCamera(){
        stopReadThread();

        if (null != mCaptureSession) {
            mCaptureSession.close();
            mCaptureSession = null;
        }
        if (null != mCameraDevice) {
            mCameraDevice.close();
            mCameraDevice = null;
        }
        if (null != mImageReader) {
            mImageReader.close();
            mImageReader = null;
        }
        previewSurface = null;

        if (videoEncoder != null){
            videoEncoder.stop();
        }

        StreamMediaNative.release();
    }

    public void startPreview(){
        if (mBackgroudHandler != null){
            mBackgroudHandler.post(new Runnable() {
                @Override
                public void run() {
                    createCameraPreviewSession();
                }
            });
        }
    }

    private ImageUtil mImageUtil = new ImageUtil();
    private final ImageReader.OnImageAvailableListener mOnImageAvailableListener
            = new ImageReader.OnImageAvailableListener() {

        @Override
        public void onImageAvailable(ImageReader reader) {
//            MLog.i("onImageAvailable");
//            mBackgroundHandler.post(new ImageSaver(reader.acquireNextImage(), mFile));
//            startReadThread();
            Image image = reader.acquireNextImage();
            if (image != null){
//                MLog.i("image != null");
                byte[] yuvData = mImageUtil.getBytesFromImageAsType(image,ImageUtil.YUV420P);

//                if (videoEncoder != null && videoEncoder.isWorking()){
//                    videoEncoder.inputData(yuvData);
//                }
               if (yuvData!=null && yuvData.length > 0){
//                    MLog.d("yuvData len : "+yuvData.length);
                   TestCameraActivity.openGlEs.sendYuvData(0,yuvData,yuvData.length);
               }else {
                   MLog.i("yuv data empty");
               }
//                StreamMediaNative.pushVideoData(yuvData,null);



                image.close();
            }else {
                MLog.i("image == null");
            }


        }

    };

    private void createImageReader(){
        MLog.d("createImageReader");
        CameraManager manager = (CameraManager) mContext.getSystemService(Context.CAMERA_SERVICE);
        try {
            CameraCharacteristics characteristics = manager.getCameraCharacteristics(curCameraId);
            StreamConfigurationMap map = characteristics.get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);
            Range[] ranges = characteristics.get(CameraCharacteristics.CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES);

            Size largest = Collections.max(Arrays.asList(map.getOutputSizes(ImageFormat.YUV_420_888)), new CompareSizesByArea());

            mImageReader = ImageReader.newInstance(640, 480, ImageFormat.YUV_420_888, /*maxImages*/10);
            mImageReader.setOnImageAvailableListener(mOnImageAvailableListener, mBackgroudHandler);
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
    }


    /**
     * Compares two {@code Size}s based on their areas.
     */
    static class CompareSizesByArea implements Comparator<Size> {

        @Override
        public int compare(Size lhs, Size rhs) {
            // We cast here to ensure the multiplications won't overflow
            return Long.signum((long) lhs.getWidth() * lhs.getHeight() -
                    (long) rhs.getWidth() * rhs.getHeight());
        }

    }


    private void readDataAction(){
        try {
            if (mImageReader.acquireLatestImage() != null){
                MLog.i("image != null");
            }else {
                MLog.i("image == null");
            }
        }catch (IllegalStateException e){
            MLog.e("IllegalStateException error ",e);
        }
    }

    private VideoParam videoParam = new VideoParam();
    private VideoEncoder videoEncoder = new VideoEncoder();

    private void initCodec(){
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT && CodecUtils.supportAvcCodec()){
            videoEncoder.setVideoParam(videoParam);
            videoEncoder.setDataCallback(null);
            videoEncoder.start();
        }
    }


    private Thread readThread;
    protected volatile boolean runFlag = false;
    private void startReadThread(){
        if (runFlag){
            MLog.i(this.getClass().getCanonicalName()+" had start");
            return;
        }
        runFlag = true;
        readThread = new Thread(new Runnable() {
            @Override
            public void run() {
                while (runFlag){
                    readDataAction();
                }
            }
        });
        readThread.setName("read data thread");
        readThread.start();
    }

    private void stopReadThread(){
        runFlag = false;
        if(readThread != null){
            try {
                readThread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    public void setVideoDataCallback(VideoDataCallback videoDataCallback){

    }

    public String getCameraId(){
        return "";
    }
}
