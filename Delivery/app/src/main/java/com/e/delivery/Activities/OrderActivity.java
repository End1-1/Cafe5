package com.e.delivery.Activities;

import android.os.Bundle;
import android.view.View;

import com.e.delivery.Data.GoodsProvider;
import com.e.delivery.Fragments.FRCostumer;
import com.e.delivery.Fragments.FROrderBill;
import com.e.delivery.Fragments.FROrderGoods;
import com.e.delivery.R;
import com.e.delivery.Utils.EnumView;

public class OrderActivity extends ParentActivity {

    FRCostumer frCostumer = new FRCostumer();
    FROrderBill frBill = new FROrderBill();
    FROrderGoods frGoods = new FROrderGoods();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_order);
        EnumView.setButtonsClickListener(findViewById(R.id.idParent), this);
        replaceFragment(frCostumer, R.id.fragment, FRCostumer.mTag);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btnCustomer:
                replaceFragment(frCostumer, R.id.fragment, FRCostumer.mTag);
                break;
            case R.id.btnGoods:
                replaceFragment(frGoods, R.id.fragment, FROrderGoods.mTag);
                break;
            case R.id.btnBill:
                replaceFragment(frBill, R.id.fragment, FROrderBill.mTag);
                break;
        }
    }

    public void readyGoods() {
        GoodsProvider.setReadyGoods();
        replaceFragment(frGoods, R.id.fragment, FROrderGoods.mTag);
    }
}
