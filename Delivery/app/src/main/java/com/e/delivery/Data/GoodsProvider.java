package com.e.delivery.Data;

import android.content.ContentValues;
import android.database.Cursor;

import com.e.delivery.Activities.DeliveryApp;
import com.e.delivery.Utils.Database;
import com.google.gson.Gson;

import org.json.JSONArray;
import org.json.JSONException;

import java.util.LinkedList;
import java.util.List;

public class GoodsProvider {
    public static List<GoodsGroup> mGoodsGroups = new LinkedList<>();
    public static List<Goods> mGoods = new LinkedList<>();
    public static List<Goods> mProxyGoods = new LinkedList<>();
    public static List<Goods> mReadyGoods = new LinkedList<>();
    public static Double mTotalAmount = 0.0;
    public static Double mCashAmount = 0.0;
    public static Double mBankAmount = 0.0;
    public static Double mDebtAmount = 0.0;

    public static void init() {
        GoodsProvider.mGoods.clear();
        GoodsProvider.mGoodsGroups.clear();
        Database db = new Database(DeliveryApp.getAppContext());
        Cursor c = db.select("select * from gg");
        if (c.moveToFirst()) {
            do {
                GoodsGroup gg = new GoodsGroup();
                gg.mCode = c.getInt(c.getColumnIndex("id"));
                gg.mName = c.getString(c.getColumnIndex("name"));
                mGoodsGroups.add(gg);
            } while (c.moveToNext());
        }
        c = db.select("select * from g");
        if (c.moveToFirst()) {
            do {
                Goods g = new Goods();
                g.mCode = c.getInt(c.getColumnIndex("id"));
                g.mName = c.getString(c.getColumnIndex("name"));
                g.mGroup = c.getInt(c.getColumnIndex("gg"));
                g.mRetailPrice = c.getDouble(c.getColumnIndex("price1"));
                g.mWholesalePrice = c.getDouble(c.getColumnIndex("price2"));
                mGoods.add(g);
            } while (c.moveToNext());
        }
        db.close();
        filterGoods(0);
    }

    public static void initGoodsGroups(JSONArray ja) {
        GoodsProvider.mGoodsGroups.clear();
        Database db = new Database(DeliveryApp.getAppContext());
        db.exec("delete from gg");
        try {
            Gson g = new Gson();
            for (int i = 0; i < ja.length(); i++) {
                GoodsGroup gg = g.fromJson(ja.getJSONObject(i).toString(), GoodsGroup.class);
                GoodsProvider.mGoodsGroups.add(gg);
                ContentValues cv = db.getContentValues();
                cv.put("id", gg.mCode);
                cv.put("name", gg.mName);
                db.insert("gg");
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }
        db.close();
    }

    public static void initGoods(JSONArray ja) {
        GoodsProvider.mGoods.clear();
        Database db = new Database(DeliveryApp.getAppContext());
        db.exec("delete from g");
        try {
            Gson g = new Gson();
            for (int i = 0; i < ja.length(); i++) {
                Goods gg = g.fromJson(ja.getJSONObject(i).toString(), Goods.class);
                GoodsProvider.mGoods.add(gg);
                ContentValues cv = db.getContentValues();
                cv.put("id", gg.mCode);
                cv.put("gg", gg.mGroup);
                cv.put("name", gg.mName);
                cv.put("price1", gg.mRetailPrice);
                cv.put("price2", gg.mWholesalePrice);
                db.insert("g");
            }
            filterGoods(0);
        } catch (JSONException e) {
            e.printStackTrace();
        }
        db.close();
    }

    public static void filterGoods(int type) {
        mProxyGoods.clear();
        for (Goods g: mGoods) {
            if (type > 0) {
                if (g.mGroup != type) {
                    continue;
                }
            }
            mProxyGoods.add(g);
        }
    }

    public static Goods getGoods(int code) {
        for (Goods g: mGoods) {
            if (g.mCode == code) {
                return g;
            }
        }
        return null;
    }

    public static void setReadyGoods() {
        mReadyGoods.clear();
        for (Goods g: mProxyGoods) {
            if (g.mSelectedQty > 0.001) {
                mReadyGoods.add(g);
                mTotalAmount += g.mSelectedQty * g.mRetailPrice;
            }
        }
        mCashAmount = 0.0;
        mBankAmount = 0.0;
        mDebtAmount = 0.0;
    }

    public static int proxyGoodsCount() {
        return mProxyGoods.size();
    }

    public static int readyGoodsCount() {
        return mReadyGoods.size();
    }

    public static void clearOrder() {
        for (Goods g: mGoods) {
            g.mSelectedQty = 0.0;
        }
        mReadyGoods.clear();
        mTotalAmount = 0.0;
        mCashAmount = 0.0;
        mBankAmount = 0.0;
        mDebtAmount = 0.0;
    }
}
