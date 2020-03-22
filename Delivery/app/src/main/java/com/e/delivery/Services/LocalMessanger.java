package com.e.delivery.Services;

import android.content.Intent;

import androidx.localbroadcastmanager.content.LocalBroadcastManager;

import com.e.delivery.Activities.DeliveryApp;
import com.e.delivery.Data.DataMessage;

public class LocalMessanger {
    public static void sendMessage(DataMessage m) {
        Intent i = new Intent(m.mReceiver);
        i.putExtra("datamessage", m);
        LocalBroadcastManager.getInstance(DeliveryApp.getAppContext()).sendBroadcast(i);
    }
}
