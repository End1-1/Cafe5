package com.e.delivery.Services;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.database.Cursor;
import android.util.Log;

import androidx.localbroadcastmanager.content.LocalBroadcastManager;

import com.e.delivery.Activities.DeliveryApp;
import com.e.delivery.Data.DataMessage;
import com.e.delivery.Utils.DataSenderCommands;
import com.e.delivery.Utils.Database;
import com.e.delivery.Utils.Json;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.LinkedList;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

public class OrderUpload {

    public Timer mTimer;
    static boolean mCanUpload = true;

    public OrderUpload() {
    }

    public void start() {
        mTimer = new Timer();
        mTimer.schedule(new Task(), 10000, 10000);
        LocalBroadcastManager.getInstance(DeliveryApp.getAppContext()).registerReceiver(mMessageHandler, new IntentFilter("OU"));
    }

    public void stop() {
        mTimer.cancel();
        LocalBroadcastManager.getInstance(DeliveryApp.getAppContext()).unregisterReceiver(mMessageHandler);
    }

    public void disableOrder(String id) {
        DataMessage m = new DataMessage(DataSenderCommands.lDisableOrder, id, "OU", "A");
        LocalMessanger.sendMessage(m);
    }

    BroadcastReceiver mMessageHandler = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            DataMessage m = intent.getParcelableExtra("datamessage");
            if (m.mResponse == DataSenderCommands.rOk) {
                Json j = new Json(m.mBuffer);
                Database db = new Database(DeliveryApp.getAppContext());
                String sql = "";
                switch (m.mCommand) {
                    case DataSenderCommands.qUploadSale:
                        sql = String.format("update oh set unid='%s' where upid='%s'", j.getString("uuid"), j.getString("id"));
                        break;
                    case DataSenderCommands.qWonnaUUID:
                        sql = String.format("update oh set upid='%s' where id=%s and upid is null", j.getString("uuid"), j.getString("id"));
                        disableOrder(j.getString("id"));
                        break;
                }
                db.exec(sql);
                db.close();
            }
            mCanUpload = true;
        }
    };

    class Task extends TimerTask {

        @Override
        public void run() {
            if (!mCanUpload) {
                return;
            }
            Database db = new Database(DeliveryApp.getAppContext());
            Cursor c = db.select("select id from oh where upid is null");
            if (c.moveToFirst()) {
                do {
                    try {
                        JSONObject j = new JSONObject();
                        j.put("id", c.getInt(c.getColumnIndex("id")));
                        mCanUpload = false;
                        DataMessage m = new DataMessage(DataSenderCommands.qWonnaUUID, j.toString(), "OU", "S");
                        LocalMessanger.sendMessage(m);
                    } catch (JSONException e) {
                        e.printStackTrace();
                    }
                } while (c.moveToNext());
            }
            c = db.select("select id from oh where upid is not null and unid is null");
            List<Integer> ids = new LinkedList<>();
            if (c.moveToFirst()) {
                do {
                    ids.add(c.getInt(c.getColumnIndex("id")));
                } while (c.moveToNext());
            }
            for (Integer id: ids) {
                try {
                    JSONObject jo = new JSONObject();
                    Log.d("ORDER TO SEND", id.toString());
                    c = db.select(String.format("select * from oh where id=%d", id));
                    if (c.moveToFirst()) {
                        JSONObject jh = new JSONObject();
                        jh.put("id", c.getInt(c.getColumnIndex("id")));
                        jh.put("unid", c.getString(c.getColumnIndex("upid")));
                        jh.put("taxcode", c.getString(c.getColumnIndex("taxcode")));
                        jh.put("atotal", c.getDouble(c.getColumnIndex("atotal")));
                        jh.put("acash", c.getDouble(c.getColumnIndex("acash")));
                        jh.put("abank", c.getDouble(c.getColumnIndex("abank")));
                        jh.put("adept", c.getDouble(c.getColumnIndex("adept")));
                        jh.put("add", c.getString(c.getColumnIndex("coordinates")));
                        jo.put("h", jh);
                    }
                    c = db.select(String.format("select * from ob where oh=%d", id));
                    JSONArray jab = new JSONArray();
                    if (c.moveToFirst()) {
                        do {
                            JSONObject jb = new JSONObject();
                            jb.put("goods", c.getInt(c.getColumnIndex("goods")));
                            jb.put("qty", c.getDouble(c.getColumnIndex("qty")));
                            jb.put("price", c.getDouble(c.getColumnIndex("price")));
                            jab.put(jb);
                        } while (c.moveToNext());
                    }
                    jo.put("b", jab);
                    mCanUpload = false;
                    DataMessage m = new DataMessage(DataSenderCommands.qUploadSale, jo.toString(), "OU", "S");
                    LocalMessanger.sendMessage(m);
                } catch (JSONException e) {
                    e.printStackTrace();
                }
            }
            db.close();
        }
    }
}
