package com.e.delivery.Activities;

import android.content.ContentValues;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.view.View;

import androidx.appcompat.app.AlertDialog;

import com.e.delivery.Data.Goods;
import com.e.delivery.Data.GoodsProvider;
import com.e.delivery.Data.Partner;
import com.e.delivery.Fragments.FRCustomer;
import com.e.delivery.Fragments.FROrderBill;
import com.e.delivery.Fragments.FROrderGoods;
import com.e.delivery.Fragments.FRSearchPartner;
import com.e.delivery.R;
import com.e.delivery.Utils.CompareDouble;
import com.e.delivery.Utils.Database;
import com.e.delivery.Utils.EnumView;

public class OrderActivity extends ParentActivity {

    FRCustomer frCustomer = new FRCustomer();
    FROrderBill frBill = new FROrderBill();
    FROrderGoods frGoods = new FROrderGoods();
    Partner mPartner;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_order);
        EnumView.setButtonsClickListener(findViewById(R.id.idParent), this);
        replaceFragment(frCustomer, R.id.fragment);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btnCustomer:
                replaceFragment(frCustomer, R.id.fragment);
                break;
            case R.id.btnGoods:
                replaceFragment(frGoods, R.id.fragment);
                break;
            case R.id.btnBill:
                replaceFragment(frBill, R.id.fragment);
                break;
        }
    }

    public void readyGoods() {
        GoodsProvider.setReadyGoods();
        replaceFragment(frGoods, R.id.fragment);
    }

    public void searchPartner() {
        replaceFragment(new FRSearchPartner(), R.id.fragment);
    }

    public void setPartner(Partner p) {
        mPartner = p;
        replaceFragment(frCustomer, R.id.fragment);
    }

    public Partner getPartner() {
        return mPartner;
    }

    public void saveOrder() {
        String err = "";
        if (mPartner == null) {
            err += getString(R.string.PartnerIsUndefined) + "\r\n";
        }
        if (GoodsProvider.mReadyGoods.size() == 0) {
            err += getString(R.string.GoodsListIsEmpty) + "\r\n";
        }
        if (!CompareDouble.eq(GoodsProvider.mTotalAmount, GoodsProvider.mCashAmount + GoodsProvider.mBankAmount + GoodsProvider.mDebtAmount)) {
            err += getString(R.string.InvalidTotalAmount) + "\r\n";
        }
        if (!err.isEmpty()) {
            AlertDialog.Builder ab = new AlertDialog.Builder(this);
            ab.setMessage(err);
            ab.setCancelable(false);
            ab.setNegativeButton(R.string.Close, new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    dialog.cancel();
                }
            });
            AlertDialog dlg = ab.create();
            dlg.show();
            return;
        }
        Database db = new Database(this);
        db.beginTransaction();
        try {
            ContentValues cv = db.getContentValues();
            cv.put("taxcode", mPartner.mTaxCode);
            cv.put("customer", mPartner.mMarketName + ", " + mPartner.mAddress);
            cv.put("atotal", GoodsProvider.mTotalAmount);
            cv.put("acash", GoodsProvider.mCashAmount);
            cv.put("abank", GoodsProvider.mBankAmount);
            cv.put("adept", GoodsProvider.mDebtAmount);
            int id = db.insertWithId("oh");
            for (int i = 0; i < GoodsProvider.mReadyGoods.size(); i++) {
                Goods g = GoodsProvider.mReadyGoods.get(i);
                cv.put("oh", id);
                cv.put("goods", g.mCode);
                cv.put("qty", g.mSelectedQty);
                cv.put("price", g.mRetailPrice);
                db.insert("ob");
            }
            db.commitTransactoin();
        } finally {
            db.endTransaction();
        }
        db.close();
        AlertDialog.Builder ab = new AlertDialog.Builder(this);
        ab.setMessage(R.string.OrderSaved);
        ab.setCancelable(false);
        ab.setNegativeButton(R.string.Close, new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                OrderActivity.this.onBackPressed();
            }
        });
        AlertDialog dlg = ab.create();
        dlg.show();
    }
}
