package com.e.delivery.Data;

import com.google.gson.annotations.SerializedName;

public class Goods {
    @SerializedName("f_id")
    public int mCode;

    @SerializedName("f_group")
    public int mGroup;

    @SerializedName("f_name")
    public String mName;

    @SerializedName("f_saleprice")
    public double mRetailPrice;

    @SerializedName("f_saleprice2")
    public double mWholesalePrice;

    public double mSelectedQty;

    public Goods() {
        mRetailPrice = 0;
        mWholesalePrice = 0;
        mSelectedQty = 0;
    }
}