package com.stream.media.view;

import android.content.Context;
import android.opengl.Matrix;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.text.TextUtils;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.lib.commonlib.utils.MLog;
import com.stream.media.jni.MediaJni;
import com.stream.media.jni.PlayerParam;


@RequiresApi(Build.VERSION_CODES.LOLLIPOP)
public class PlayerDisplayView extends SurfaceView implements SurfaceHolder.Callback{
    private SurfaceHolder.Callback holdCallback;

    private MediaJni mediaJni;
    private PlayerParam playerParam;

    private float[] mViewMatrix=new float[16];
    private float[] mProjectMatrix=new float[16];
    private float[] mMVPMatrix=new float[16];

    public PlayerDisplayView(Context context) {
        super(context);
        init();
    }

    public PlayerDisplayView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public PlayerDisplayView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }

    public PlayerDisplayView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        init();
    }

    void init(){
        getHolder().addCallback(this);
        initMetrix();
        playerParam = new PlayerParam();
        playerParam.matrix = mMVPMatrix;
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        MLog.d("player surfaceCreated width : "+ getWidth()+ " , height : "+ getHeight());
        if (mediaJni != null && !TextUtils.isEmpty(playerParam.url)&& !TextUtils.isEmpty(playerParam.id)){
            playerParam.videoWidth = getWidth();
            playerParam.videoHeight = getHeight();
            playerParam.viewWidth = getWidth();
            playerParam.viewHeight = getHeight();
            playerParam.videoWidth = getWidth();

            mediaJni.createPlayer(playerParam.id);
            mediaJni.setPlayerParam(playerParam.id,playerParam);
            mediaJni.startPlay(playerParam.id);
        }

        if (holdCallback != null){
            holdCallback.surfaceCreated(holder);
        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        MLog.i("player  surfaceChanged width : "+ width+ " , height : "+ height);

        if (holdCallback != null){
            holdCallback.surfaceChanged(holder, format,width,height);
        }
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        MLog.i("player  surfaceDestroyed");
        if (mediaJni != null && !TextUtils.isEmpty(playerParam.url)&& !TextUtils.isEmpty(playerParam.id)){
            mediaJni.stopPlay(playerParam.id);
        }

        if (holdCallback != null){
            holdCallback.surfaceDestroyed(holder);
        }
    }



    private void initMetrix(){
        Matrix.orthoM(mProjectMatrix,0,-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 2.0f);

//            Matrix.orthoM(mProjectMatrix,0,);
        Matrix.rotateM(mProjectMatrix,0,270.0f,0.0f,0.0f,1.0f);

        //设置相机位置
        Matrix.setLookAtM(mViewMatrix, 0, 0, 0, 2.0f, 0f, 0f, 0f, 0f, 1.0f, 0.0f);
        //计算变换矩阵
        Matrix.multiplyMM(mMVPMatrix,0,mProjectMatrix,0,mViewMatrix,0);
    }

    public void setHoldCallback(SurfaceHolder.Callback holdCallback) {
        this.holdCallback = holdCallback;
    }

    public void setMediaJni(MediaJni mediaJni) {
        this.mediaJni = mediaJni;
    }

    public PlayerParam getPlayerParam() {
        return playerParam;
    }
}
