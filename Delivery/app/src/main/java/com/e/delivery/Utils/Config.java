package com.e.delivery.Utils;
import android.content.Context;
import android.content.SharedPreferences;

public final class Config {

    public static String mServerIP;
    public static int mServerPort;
    private static final String prefName  = "com.e.delivery";

    public static void init(Context context) {
        mServerIP = getString(context,"server_ip");
        mServerPort = getInt(context,"server_port");
    }

    private static SharedPreferences pref(Context context) {
        return context.getSharedPreferences(prefName, Context.MODE_PRIVATE);
    }

    public static boolean getBoolean(Context context, String key) {
        return pref(context).getBoolean(key, false);
    }

    public static void setBoolean(Context context, String key, boolean value) {
        SharedPreferences.Editor e = pref(context).edit();
        e.putBoolean(key, value);
        e.commit();
    }

    public static float getFloat(Context context, String key) {
        return pref(context).getFloat(key, 0);
    }

    public static void setFloat(Context context, String key, float value) {
        SharedPreferences.Editor e = pref(context).edit();
        e.putFloat(key, value);
        e.commit();
    }

    public static long getLong(Context context, String key) {
        return pref(context).getLong(key, 0);
    }

    public static void setLong(Context context, String key, long value) {
        SharedPreferences.Editor e = pref(context).edit();
        e.putLong(key, value);
        e.commit();
    }

    public static int getInt(Context context, String key) {
        return pref(context).getInt(key, 0);
    }

    public static void setInt(Context context, String key, int value) {
        SharedPreferences.Editor e = pref(context).edit();
        e.putInt(key, value);
        e.commit();
    }

    public static String getString(Context context, String key) {
        return pref(context).getString(key, "");
    }

    public static void setString(Context context, String key, String value) {
        SharedPreferences.Editor e = pref(context).edit();
        e.putString(key, value);
        e.commit();
    }

}
