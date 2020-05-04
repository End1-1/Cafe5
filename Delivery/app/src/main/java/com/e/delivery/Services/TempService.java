package com.e.delivery.Services;

import android.annotation.SuppressLint;
import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.location.LocationManager;
import android.media.RingtoneManager;
import android.os.Build;
import android.os.IBinder;

import androidx.annotation.Nullable;
import androidx.core.app.NotificationCompat;
import androidx.localbroadcastmanager.content.LocalBroadcastManager;

import com.e.delivery.Activities.DeliveryApp;
import com.e.delivery.Activities.MainActivity;
import com.e.delivery.Data.DataMessage;
import com.e.delivery.R;
import com.e.delivery.Utils.Config;
import com.e.delivery.Utils.DataSenderCommands;
import com.e.delivery.Utils.Json;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.net.SocketTimeoutException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.security.KeyManagementException;
import java.security.KeyStore;
import java.security.KeyStoreException;
import java.security.NoSuchAlgorithmException;
import java.security.UnrecoverableKeyException;
import java.security.cert.CertificateException;
import java.util.LinkedList;
import java.util.NoSuchElementException;
import java.util.Queue;

import javax.net.ssl.KeyManagerFactory;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLSocket;
import javax.net.ssl.SSLSocketFactory;
import javax.net.ssl.TrustManager;
import javax.net.ssl.TrustManagerFactory;

import static android.app.Notification.DEFAULT_VIBRATE;

public class TempService extends Service {

    static final String CHANNEL_ID = "1250012";
    static final String TAG = TempService.class.getSimpleName();
    OrderUpload mOrderUpload = new OrderUpload();
    MessageHandler mMessageHandler;
    Queue<DataMessage> mDataMessage = new LinkedList<>();
    LocationService mLocationService = new LocationService();

    @Override
    public void onCreate() {
        super.onCreate();
        Bitmap icon = BitmapFactory.decodeResource(getResources(), R.mipmap.ic_launcher);
        Intent intent = new Intent(this, MainActivity.class);
        PendingIntent pendingIntent = PendingIntent.getActivity(this, 0, intent, PendingIntent.FLAG_ONE_SHOT);
        NotificationCompat.Builder notificationBuilder;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            notificationBuilder = new NotificationCompat.Builder(this, CHANNEL_ID);
            NotificationChannel notificationChannel = new NotificationChannel(CHANNEL_ID, TAG, NotificationManager.IMPORTANCE_DEFAULT);
            notificationChannel.enableVibration(true);
            ((NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE)).createNotificationChannel(notificationChannel);
        } else {
            notificationBuilder =  new NotificationCompat.Builder(this);
        }
        notificationBuilder
//                .setContentTitle(notification.getTitle())
                .setContentText(getString(R.string.DeliveryAgent))
                // .setDefaults(DEFAULT_SOUND | DEFAULT_VIBRATE)
                .setAutoCancel(true)
                .setSound(RingtoneManager.getDefaultUri(RingtoneManager.TYPE_NOTIFICATION))
                .setContentIntent(pendingIntent)
                .setLargeIcon(icon)
                .setColor(Color.RED)
                .setSmallIcon(R.mipmap.ic_launcher);

        notificationBuilder.setDefaults(DEFAULT_VIBRATE);
        notificationBuilder.setLights(Color.YELLOW, 1000, 300);

        NotificationManager notificationManager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
        notificationManager.notify(0, notificationBuilder.build());

        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.O) {
            NotificationChannel channel = new NotificationChannel(CHANNEL_ID, TAG, NotificationManager.IMPORTANCE_HIGH);
            notificationManager.createNotificationChannel(channel);
            Notification notification = new Notification.Builder(getApplicationContext(),CHANNEL_ID).build();
            startForeground(1, notification);
        }
        else {
            // startForeground(1, notification);
        }
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @SuppressLint("MissingPermission")
    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        super.onStartCommand(intent, flags, startId);
        //ExecutorService es = Executors.newFixedThreadPool(10);
        mMessageHandler = new MessageHandler();
        new Thread(mMessageHandler).start();
        //mServerThreadFuture = es.submit(mSocketThread);
        LocalBroadcastManager.getInstance(this).registerReceiver(mMessageReceiver, new IntentFilter("S"));
        mLocationService = new LocationService();
        LocationManager locationManager = (LocationManager) getSystemService(LOCATION_SERVICE);
        //locationManager.requestSingleUpdate(LocationManager.GPS_PROVIDER, mLocationService, this.getMainLooper());
        locationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER, 1000, 1, mLocationService);
        return START_STICKY;
    }

    private BroadcastReceiver mMessageReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            DataMessage m = intent.getParcelableExtra("datamessage");
            if (m.mSender.equals(m.mReceiver) && m.mSender.equals("S")) {
                if (m.mResponse == 0) {
                    mDataMessage.add(m);
                }
            } else {
                mDataMessage.add(m);
            }
        }
    };

    class MessageHandler implements Runnable {

        SSLSocketFactory mSSLSocketFactory = null;
        SSLSocket mSocket = null;
        boolean mConnected = false;
        boolean mAuthorized = false;

        void connect() {
            if (mConnected) {
                return;
            }
            mConnected = false;
            String err = "";
            do {
                try {
                    if (mSocket != null) {
                        mSocket.close();
                        mSocket = null;
                    }
                    if (mSSLSocketFactory == null) {
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
                        sslContext.init(kmf.getKeyManagers(), tm, null);
                        mSSLSocketFactory = sslContext.getSocketFactory();
                    }

                    mSocket = (SSLSocket) mSSLSocketFactory.createSocket();
                    mSocket.connect(new InetSocketAddress(Config.mServerIP, Config.mServerPort), 15000);
                    mSocket.setSoTimeout(10000);
                    mSocket.startHandshake();
                    mConnected = true;
                } catch (IOException e) {
                    err = String.format("{\"msg\":\"%s\"}", e.getMessage());
                    e.printStackTrace();
                } catch (KeyManagementException e) {
                    err = String.format("{\"msg\":\"%s\"}", e.getMessage());
                    e.printStackTrace();
                } catch (NoSuchAlgorithmException e) {
                    err = String.format("{\"msg\":\"%s\"}", e.getMessage());
                    e.printStackTrace();
                } catch (CertificateException e) {
                    err = String.format("{\"msg\":\"%s\"}", e.getMessage());
                    e.printStackTrace();
                } catch (KeyStoreException e) {
                    err = String.format("{\"msg\":\"%s\"}", e.getMessage());
                    e.printStackTrace();
                } catch (UnrecoverableKeyException e) {
                    err = String.format("{\"msg\":\"%s\"}", e.getMessage());
                    e.printStackTrace();
                }
                try {
                    if (!mConnected) {
                        while (mDataMessage.size() > 0) {
                            DataMessage m = mDataMessage.remove();
                            m.mResponse = DataSenderCommands.rErr;
                            m.mBuffer = err;
                            m.rotate();
                            LocalMessanger.sendMessage(m);
                        }
                        Thread.sleep(1000);
                    }
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            } while (!mConnected);
            mOrderUpload.start();
            LocalMessanger.sendMessage(new DataMessage(DataSenderCommands.lServiceStarted, "", "S", "A"));
        }

        @Override
        public void run() {
            do {
                if (!mConnected) {
                    connect();
                }
                DataMessage m = null;
                try {
                    m = mDataMessage.remove();
                    if (m.mCommand != DataSenderCommands.qLogin) {
                        if (!mAuthorized) {
                            m.rotate();
                            m.mResponse = DataSenderCommands.rLoginRequired;
                            m.mBuffer = "Login required";
                            LocalMessanger.sendMessage(m);
                            if (!Config.getString("username").isEmpty()) {
                                Json j = new Json();
                                j.putString("username", Config.getString("username"));
                                j.putString("password", Config.getString("password"));
                                j.putString("session", Config.getString("session_id"));
                                m = new DataMessage(DataSenderCommands.qLogin, j.toString(), "A", "S");
                            }
                        }
                    }
                } catch (NoSuchElementException e) {
                    m = null;
                    e.printStackTrace();
                    try {
                        Thread.sleep(1000);
                    } catch (InterruptedException ee) {
                        ee.printStackTrace();
                    }
                }
                try {
                    ByteBuffer bb = ByteBuffer.allocate(8);
                    if (m != null) {
                        m.rotate();
                        OutputStream os = mSocket.getOutputStream();
                        DataOutputStream dos = new DataOutputStream(os);
                        bb.order(ByteOrder.LITTLE_ENDIAN);
                        bb.putInt(m.mBuffer.getBytes("UTF-8").length);
                        bb.putInt(m.mCommand);
                        byte[] bytes = bb.array();
                        dos.write(bytes, 0, 8);
                        dos.flush();
                        bytes = m.mBuffer.getBytes("UTF-8");
                        dos.write(bytes, 0, bytes.length);
                        dos.flush();
                    }
                    InputStream is = mSocket.getInputStream();
                    DataInputStream dis = new DataInputStream(is);
                    byte[] b = new byte[4];
                    int read = dis.read(b, 0, 4);
                    bb.clear();
                    bb.put(b);
                    dis.read(b, 0, 4);
                    bb.put(b);
                    bb.position(0);
                    Integer datasize = bb.getInt();
                    if (m == null) {
                        m = new DataMessage(0, "", "S", "S");
                        m.mResponse = DataSenderCommands.rOk;
                    }
                    if (read  == -1) {
                        m.mCommand = DataSenderCommands.rReconnect;
                        m.mResponse = DataSenderCommands.rErr;
                    }
                    m.mResponse = bb.getInt();
                    m.mBuffer = "";
                    while (dis.available() > 0 || datasize > 0) {
                        int pt;
                        byte[] bbb = new byte[8192];
                        pt = dis.read(bbb, 0, 8192);
                        datasize -= pt;
                        m.mBuffer += new String(bbb, 0, pt);
                    }
                    if (m.mResponse == DataSenderCommands.rLoginRequired) {
                        mAuthorized = false;
                        if (Config.getString("username").length() > 0) {
                            Json j = new Json();
                            j.putString("username", Config.getString("username"));
                            j.putString("password", Config.getString("password"));
                            DataMessage ml = new DataMessage(DataSenderCommands.qLogin, j.toString(), "S", "S");
                            LocalMessanger.sendMessage(ml);
                        }
                    }
                    if (m.mCommand == DataSenderCommands.qLogin) {
                        if (m.mResponse == DataSenderCommands.rOk) {
                            mAuthorized = true;
                        }
                    }
                } catch (SocketTimeoutException e) {
                    if (m == null) {
                        m = new DataMessage(0, "", "", "");
                    }
                    if (m.mCommand == 0) {
                        m.mCommand = DataSenderCommands.rIgnore;
                    } else {
                        m.mResponse = DataSenderCommands.rErr;
                        m.mBuffer = String.format("{\"msg\":\"%s\"}", e.getMessage());
                    }
                } catch (IOException e) {
                    if (m == null) {
                        m = new DataMessage(0, "", "", "");
                    }
                    m.mResponse = DataSenderCommands.rErr;
                    m.mBuffer = String.format("{\"msg\":\"%s\"}", e.getMessage());
                    e.printStackTrace();
                }
                if (m.mCommand != DataSenderCommands.rIgnore) {
                    LocalMessanger.sendMessage(m);
                }
                if (m.mCommand == DataSenderCommands.rReconnect) {
                    mConnected = false;
                    mAuthorized = false;
                    mOrderUpload.stop();
                    connect();
                }
            } while (true);
        }
    }
}
