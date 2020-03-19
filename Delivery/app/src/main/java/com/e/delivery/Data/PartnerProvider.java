package com.e.delivery.Data;

import com.google.gson.Gson;

import org.json.JSONArray;
import org.json.JSONException;

import java.util.LinkedList;
import java.util.List;

public class PartnerProvider {
    public static List<Partner> mPartners = new LinkedList<>();
    public static List<Partner> mProxyPartners = new LinkedList<>();

    public static void initPartners(JSONArray ja) {
        mPartners.clear();
        try {
            Gson gson = new Gson();
            for (int i = 0; i < ja.length(); i++) {
                Partner partner = gson.fromJson(ja.getJSONObject(i).toString(), Partner.class);
                mPartners.add(partner);
                mProxyPartners.add(partner);
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }
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
