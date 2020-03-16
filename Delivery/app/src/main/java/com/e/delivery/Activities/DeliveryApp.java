package com.e.delivery.Activities;

import android.app.Application;
import android.content.Context;

import com.e.delivery.Utils.Config;

public class DeliveryApp extends Application {

    static Context mAppContext;

    public static Context getAppContext() {
        return mAppContext;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        Config.init(getApplicationContext());
        mAppContext = getApplicationContext();
    }
}
