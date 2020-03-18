package com.e.delivery.Data;

import com.google.gson.annotations.SerializedName;

public class Partner {
    @SerializedName("f_taxcode")
    public String mTaxCode;
    @SerializedName("f_taxname")
    public String mTaxName;
    @SerializedName("f_info")
    public String mMarketName;
    @SerializedName("f_contact")
    public String mContact;
    @SerializedName("f_phone")
    public String mPhone;
    @SerializedName("f_address")
    public String mAddress;
}
