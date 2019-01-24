package com.stream.media.demo.network;

import com.lib.commonlib.utils.bytes.ByteTools;
import com.stream.media.demo.utils.MLog;

import java.io.DataInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.util.concurrent.ArrayBlockingQueue;

/**
 * Created by dengjun on 2019/1/24.
 */

public class TcpClientTest {
    private NetDataCallback netDataCallback;

    private Socket socket;
    private OutputStream outputStream;
    private InputStream inputStream;


    private volatile boolean loopFlag = false;
    private Thread readThread;
    private Thread sendThread;

    private byte[] buffer = new byte[1920*100];

    private  ArrayBlockingQueue<byte[]> h264DataQueue = new ArrayBlockingQueue<byte[]>(30);


    public void start(){
        loopFlag = true;
        sendThread = new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    socket = new Socket();
                    socket.setSoTimeout(4*60*1000);
                    socket.connect(new InetSocketAddress("192.168.3.71",8888), 10*1000);
                    socket.setReceiveBufferSize(1920*100);
                    socket.setReceiveBufferSize(1920*100);
                    inputStream = socket.getInputStream();
                    outputStream = socket.getOutputStream();

                    startReadThread();

                    while (loopFlag){
                        byte[] data = null;
                        try {
                            data = h264DataQueue.take();
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }

                        if (data != null && socket.isConnected()&& outputStream != null){
//                            MLog.i("send data len :"+data.length);
                            outputStream.write(ByteTools.intToByteArray(data.length));
                            outputStream.flush();
                            outputStream.write(data);
                            outputStream.flush();
                        }
                    }
                }catch (IOException e){
                    e.printStackTrace();
                    MLog.e("",e);
                    loopFlag = false;
                }



                stopReadThread();
            }
        });
        sendThread.setName("readThread");
        sendThread.start();
    }

    public void stop(){
        if (sendThread != null){
            try {
                loopFlag = false;
                h264DataQueue.notifyAll();
                sendThread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            sendThread = null;
        }
    }


    private void startReadThread(){
        loopFlag = true;
        readThread = new Thread(new Runnable() {
            @Override
            public void run() {
                int pos = 0;
                byte[] h264Data = null;

                ByteBuffer byteBuffer = ByteBuffer.allocate(1920*100);
                int h264len = 0;
                while (loopFlag){
                    try {
                        int len = inputStream.read(buffer);
//                        MLog.i("read data len :"+len+"  ,  h264 len : "+(int)ByteTools.parseIntFromArrayAsBig(buffer,0));
                        if (h264len == 0){
                            h264len = (int)ByteTools.parseIntFromArrayAsBig(buffer,0);
//                            MLog.i("1  h264 len : "+ h264len);
                        }
                        if (len > 0){
                            byteBuffer.put(buffer,0,len);

                            if (byteBuffer.position() >= h264len+4){
                                h264Data = new byte[h264len];
                                byteBuffer.flip();
                                byteBuffer.position(4);
                                byteBuffer.get(h264Data,0,h264len);
                                if (netDataCallback != null){
                                    netDataCallback.onData(h264Data,0,h264Data.length);
                                }

                                if (byteBuffer.remaining() >= 4){
                                    byte[] lenData = new byte[4];
                                    byteBuffer.get(lenData);
                                    h264len = (int)ByteTools.parseIntFromArrayAsBig(lenData,0);
//                                    MLog.i("2  h264 len : "+ h264len);

                                    byteBuffer.position(byteBuffer.position() -4);
                                }else {
                                    h264len = 0;
                                }

                                ByteBuffer remainByteBuffer = byteBuffer.slice();
                                byteBuffer.clear();
                                byteBuffer.put(remainByteBuffer);
                            }
                        }
                    } catch (IOException e) {
                        e.printStackTrace();
                        MLog.e(",",e);
                    }
                }
            }
        });
        readThread.setName("readThread");
        readThread.start();
    }

    private void stopReadThread(){
        if (readThread != null){
            try {
                loopFlag = false;
                readThread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            readThread = null;
        }
    }


    public void sendData(byte[] data,int pos,int len){
        h264DataQueue.add(data);
    }

    public void setNetDataCallback(NetDataCallback netDataCallback) {
        this.netDataCallback = netDataCallback;
    }
}
