package com.e.delivery.Utils;

import org.json.JSONException;
import org.json.JSONObject;

public class Json {

    private JSONObject mObject;

    public Json() {
        mObject = new JSONObject();
    }

    public Json(String s) {
        try {
            mObject = new JSONObject(s);
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    public Json (JSONObject o) {
        mObject = o;
    }

    public String toString() {
        return mObject.toString();
    }

    public boolean putString(String  key, String value) {
        try {
            mObject.put(key, value);
        } catch (JSONException e) {
            e.printStackTrace();
            return false;
        }
        return true;
    }

    public String getString(String key) {
        try {
            return mObject.getString(key);
        } catch (JSONException e) {
            e.printStackTrace();
        }
        return "";
    }
}
