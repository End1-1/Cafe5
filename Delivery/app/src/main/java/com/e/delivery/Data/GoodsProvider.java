package com.e.delivery.Data;

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

    public static void initGoodsGroups(JSONArray ja) {
        GoodsProvider.mGoodsGroups.clear();
        try {
            Gson g = new Gson();
            for (int i = 0; i < ja.length(); i++) {
                GoodsGroup gg = g.fromJson(ja.getJSONObject(i).toString(), GoodsGroup.class);
                GoodsProvider.mGoodsGroups.add(gg);
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    public static void initGoods(JSONArray ja) {
        GoodsProvider.mGoods.clear();
        try {
            Gson g = new Gson();
            for (int i = 0; i < ja.length(); i++) {
                Goods gg = g.fromJson(ja.getJSONObject(i).toString(), Goods.class);
                GoodsProvider.mGoods.add(gg);
            }
            filterGoods(0);
        } catch (JSONException e) {
            e.printStackTrace();
        }
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
