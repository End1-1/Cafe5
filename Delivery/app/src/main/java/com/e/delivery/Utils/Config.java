package com.e.delivery.Utils;
import android.content.Context;
import android.content.SharedPreferences;

import com.e.delivery.Activities.DeliveryApp;

public final class Config {

    public static String mServerIP;
    public static int mServerPort;
    private static final String prefName  = "com.e.delivery";

    public static void init() {
        mServerIP = getString("server_ip");
        mServerPort = getInt("server_port");
    }

    private static SharedPreferences pref() {
        return DeliveryApp.getAppContext().getSharedPreferences(prefName, Context.MODE_PRIVATE);
    }

    public static boolean getBoolean(String key) {
        return pref().getBoolean(key, false);
    }

    public static void setBoolean(String key, boolean value) {
        SharedPreferences.Editor e = pref().edit();
        e.putBoolean(key, value);
        e.commit();
    }

    public static float getFloat(String key) {
        return pref().getFloat(key, 0);
    }

    public static void setFloat(String key, float value) {
        SharedPreferences.Editor e = pref().edit();
        e.putFloat(key, value);
        e.commit();
    }

    public static long getLong(Context context, String key) {
        return pref().getLong(key, 0);
    }

    public static void setLong(String key, long value) {
        SharedPreferences.Editor e = pref().edit();
        e.putLong(key, value);
        e.commit();
    }

    public static int getInt(String key) {
        return pref().getInt(key, 0);
    }

    public static void setInt(String key, int value) {
        SharedPreferences.Editor e = pref().edit();
        e.putInt(key, value);
        e.commit();
    }

    public static String getString(String key) {
        return pref().getString(key, "");
    }

    public static void setString(String key, String value) {
        SharedPreferences.Editor e = pref().edit();
        e.putString(key, value);
        e.commit();
    }

}
