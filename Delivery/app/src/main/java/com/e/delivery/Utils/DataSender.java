package com.e.delivery.Utils;

import android.os.AsyncTask;
import android.util.Log;

import com.e.delivery.Activities.DeliveryApp;
import com.e.delivery.R;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.security.KeyStore;

import javax.net.ssl.KeyManagerFactory;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLSocket;
import javax.net.ssl.SSLSocketFactory;
import javax.net.ssl.TrustManager;
import javax.net.ssl.TrustManagerFactory;

public class DataSender extends AsyncTask<Integer, Integer, Integer> {

    String mBuffer;
    int mCommand;
    int mResponse;
    JSONObject mReply;
    DataSenderCallback mDataSenderCallback;

    public DataSender(String buffer, int command, DataSenderCallback dataSenderCallback) {
        mBuffer = buffer;
        mCommand = command;
        mResponse  = 0;
        mReply = new JSONObject();
        mDataSenderCallback = dataSenderCallback;
    }

    @Override
    protected Integer doInBackground(Integer... integers) {
        try {
            String keyStore = KeyStore.getDefaultType();
            KeyStore trusted = KeyStore.getInstance(keyStore);
            InputStream in = DeliveryApp.getAppContext().getResources().openRawResource(R.raw.keystore);
            trusted.load(in, "qwerty".toCharArray());
            in.close();
            String keyManagerFactory = KeyManagerFactory.getDefaultAlgorithm();
            KeyManagerFactory kmf = KeyManagerFactory.getInstance(keyManagerFactory);
            kmf.init(trusted, "qwerty".toCharArray());

            TrustManagerFactory tmf = TrustManagerFactory.getInstance(TrustManagerFactory.getDefaultAlgorithm());
            tmf.init(trusted);
            TrustManager[] tm = tmf.getTrustManagers();
            SSLContext sslContext = SSLContext.getInstance("TLS");
            sslContext.init(kmf.getKeyManagers(), tm,null);
            SSLSocketFactory factory = sslContext.getSocketFactory();

            SSLSocket s = (SSLSocket)factory.createSocket();
            s.connect(new InetSocketAddress(Config.mServerIP, Config.mServerPort), 5000);
            s.setSoTimeout(3000);
            s.startHandshake();
            OutputStream os = s.getOutputStream();
            DataOutputStream dos = new DataOutputStream(os);
            ByteBuffer bb = ByteBuffer.allocate(8);
            bb.order(ByteOrder.LITTLE_ENDIAN);
            bb.putInt(mBuffer.getBytes("UTF-8").length);
            bb.putInt(mCommand);
            byte[] bytes = bb.array();
            dos.write(bytes, 0, 8);
            dos.flush();
            bytes = mBuffer.getBytes("UTF-8");
            dos.write(bytes, 0, bytes.length);
            dos.flush();
            InputStream is = s.getInputStream();
            DataInputStream dis = new DataInputStream(is);
            byte [] b = new byte[4];
            int read = dis.read(b, 0, 4);
            bb.clear();
            bb.put(b);
            dis.read(b, 0, 4);
            bb.put(b);
            bb.position(0);
            Integer datasize = bb.getInt();
            mResponse = bb.getInt();
            mBuffer = "";
            while (dis.available() > 0 || datasize > 0) {
                int pt;
                byte [] bbb = new byte[8192];
                pt = dis.read(bbb, 0, 8192);
                datasize -= pt;
                mBuffer += new String(bbb, 0, pt);
            }
        } catch (IOException e) {
            e.printStackTrace();
            try {
                mReply.put("msg", e.getMessage());
                mReply.put("reply", -1);
            } catch (JSONException e1) {
                e1.printStackTrace();
            }
            return 1;
        } catch (Exception e) {
            e.printStackTrace();
            try {
                mReply.put("msg", e.getMessage());
                mReply.put("reply", -1);
            } catch (JSONException e1) {
                e1.printStackTrace();
            }
            return 1;
        }
        try {
            mReply = new JSONObject(mBuffer);
        } catch (JSONException e) {
            e.printStackTrace();
            Log.d("BUFFER", mBuffer);
        }
        return mResponse;
    }

    @Override
    protected void onPostExecute(Integer integer) {
        super.onPostExecute(integer);
        mDataSenderCallback.finish(integer, new Json(mReply));
    }

    public interface DataSenderCallback {
        void finish(int result, Json data);
    }
}
