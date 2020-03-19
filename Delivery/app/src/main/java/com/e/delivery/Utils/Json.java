package com.e.delivery.Utils;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

public class Json {

    private JSONObject mObject;

    public Json() {
        mObject = new JSONObject();
    }

    public Json(Json j) {
        mObject = j.mObject;
    }

    public Json(String s) {
        try {
            mObject = new JSONObject(s);
        } catch (JSONException e) {
            mObject = new JSONObject();
            e.printStackTrace();
        }
    }

    public Json (JSONObject o) {
        mObject = o;
    }

    public Json (String key, JSONArray a) {
        try {
            mObject = new JSONObject();
            mObject.put(key, a);
        } catch (JSONException e) {
            e.printStackTrace();
        }
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

    public boolean putInt(String key, int value) {
        try {
            mObject.put(key, value);
        } catch (JSONException e) {
            e.printStackTrace();
            return false;
        }
        return true;
    }

    public Json getJsonObject(String key) {
        try {
            return new Json(mObject.getJSONObject(key));
        } catch (JSONException e) {
            e.printStackTrace();
        }
        return new Json();
    }

    public Json getJsonArray(String key) {
        try {
            return new Json(key, mObject.getJSONArray(key));
        } catch (JSONException e) {
            e.printStackTrace();
        }
        return new Json();
    }

    public JSONArray getArray(String key) {
        try {
            return mObject.getJSONArray(key);
        } catch (JSONException e) {
            e.printStackTrace();
            return new JSONArray();
        }
    }
}
