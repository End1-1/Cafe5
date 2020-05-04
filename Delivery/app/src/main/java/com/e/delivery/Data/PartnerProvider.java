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

public class PartnerProvider {
    public static List<Partner> mPartners = new LinkedList<>();
    public static List<Partner> mProxyPartners = new LinkedList<>();

    public static void initPartners() {
        Database db = new Database(DeliveryApp.getAppContext());
        mPartners.clear();
        mProxyPartners.clear();
        Cursor c = db.select("select * from partners");
        if (c.moveToFirst()) {
            do {
                Partner partner = new Partner();
                partner.mTaxCode = c.getString(c.getColumnIndex("taxcode"));
                partner.mTaxName = c.getString(c.getColumnIndex("taxname"));
                partner.mMarketName = c.getString(c.getColumnIndex("info"));
                partner.mAddress = c.getString(c.getColumnIndex("address"));
                partner.mContact = c.getString(c.getColumnIndex("contact"));
                partner.mPhone = c.getString(c.getColumnIndex("phone"));
                mPartners.add(partner);
                mProxyPartners.add(partner);
            } while (c.moveToNext());
        }
        db.close();
    }

    public static void initPartners(JSONArray ja) {
        mPartners.clear();
        Database db = new Database(DeliveryApp.getAppContext());
        db.exec("delete from partners");
        try {
            Gson gson = new Gson();
            for (int i = 0; i < ja.length(); i++) {
                Partner partner = gson.fromJson(ja.getJSONObject(i).toString(), Partner.class);
                ContentValues cv = db.getContentValues();
                cv.put("taxcode", partner.mTaxCode);
                cv.put("taxname", partner.mTaxName);
                cv.put("info", partner.mMarketName);
                cv.put("address", partner.mAddress);
                cv.put("contact", partner.mContact);
                cv.put("phone", partner.mPhone);
                db.insert("partners");
                mPartners.add(partner);
                mProxyPartners.add(partner);
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }
        db.close();
    }

    public static void searchInAll(String s) {
        mProxyPartners = search(mPartners, s);
    }

    public static void searchInProxy(String s) {
        mProxyPartners = search(mProxyPartners, s);
    }

    static List<Partner> search(List<Partner> src, String s) {
        List<Partner> l = new LinkedList<>();
        for (int i = 0; i < src.size(); i++) {
            Partner p = src.get(i);
            if (p.mTaxCode.toLowerCase().contains(s.toLowerCase())) {
                l.add(p);
                continue;
            }
            if (p.mTaxName.toLowerCase().contains(s.toLowerCase())) {
                l.add(p);
                continue;
            }
            if (p.mAddress.toLowerCase().contains(s.toLowerCase())) {
                l.add(p);
                continue;
            }
            if (p.mPhone.toLowerCase().contains(s.toLowerCase())) {
                l.add(p);
                continue;
            }
            if (p.mMarketName.toLowerCase().contains(s.toLowerCase())) {
                l.add(p);
                continue;
            }
        }
        return l;
    }

    public static Partner getPartner(String taxcode) {
        for (Partner p: mPartners) {
            if (p.mTaxCode.equals(taxcode)) {
                return p;
            }
        }
        return null;
    }
}
