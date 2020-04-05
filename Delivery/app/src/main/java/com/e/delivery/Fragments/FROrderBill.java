package com.e.delivery.Fragments;

import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;

import com.e.delivery.Activities.OrderActivity;
import com.e.delivery.Data.GoodsProvider;
import com.e.delivery.R;
import com.e.delivery.Utils.CompareDouble;
import com.e.delivery.Utils.EnumView;

public class FROrderBill extends FRParentOrder {

    EditText edTotalAmount;
    EditText edCash;
    EditText edBank;
    EditText edDept;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View v = inflater.inflate(R.layout.fragment_order_bill, container, false);
        EnumView.setButtonsClickListener(v, this);
        edTotalAmount = v.findViewById(R.id.edTotalAmount);
        edCash = v.findViewById(R.id.edCash);
        edBank = v.findViewById(R.id.edBankTransfer);
        edDept = v.findViewById(R.id.edDept);
        return v;
    }

    @Override
    public void onResume() {
        super.onResume();
        edTotalAmount.setText(String.format("%.0f", GoodsProvider.mTotalAmount));
        edCash.setText(String.format("%.0f", GoodsProvider.mCashAmount));
        edBank.setText(String.format("%.0f", GoodsProvider.mBankAmount));
        edDept.setText(String.format("%.0f", GoodsProvider.mDebtAmount));
        edCash.addTextChangedListener(watchCash);
        edBank.addTextChangedListener(watchBank);
        edDept.addTextChangedListener(watchDept);
    }

    @Override
    public void disableEdit(boolean v) {
        super.disableEdit(v);
        int visible = v ? View.GONE : View.VISIBLE;
        getView().findViewById(R.id.btnReady).setVisibility(visible);
    }

    @Override
    public void onPause() {
        super.onPause();
        edCash.removeTextChangedListener(watchCash);
        edBank.removeTextChangedListener(watchBank);
        edDept.removeTextChangedListener(watchDept);
    }

    public void checkTotalAmounts() {
        Double t = GoodsProvider.mCashAmount + GoodsProvider.mBankAmount + GoodsProvider.mDebtAmount;
        int color = getResources().getColor(R.color.colorBlack);
        if (t > GoodsProvider.mTotalAmount) {
            color = getResources().getColor(R.color.colorRed);
        } else if (CompareDouble.eq(t, GoodsProvider.mTotalAmount)) {
            color = getResources().getColor(R.color.colorGreen);
        }
        edCash.setTextColor(color);
        edBank.setTextColor(color);
        edDept.setTextColor(color);
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.btnCorrectCash:
                calculator(GoodsProvider.mBankAmount, GoodsProvider.mDebtAmount, edCash);
                break;
            case R.id.btnCorrectBank:
                calculator(GoodsProvider.mCashAmount, GoodsProvider.mDebtAmount, edBank);
                break;
            case R.id.btnCorrectDept:
                calculator(GoodsProvider.mCashAmount, GoodsProvider.mBankAmount, edDept);
                break;
            case R.id.btnReady:
                ((OrderActivity) mActivity).saveOrder();
                break;
        }
    }

    public void calculator(double v1, double v2, EditText ed) {
        Double v = GoodsProvider.mTotalAmount - v1 - v2;
        if (v > 0) {
            ed.setText(String.format("%.0f", v));
        }
    }

    TextWatcher watchCash = new TextWatcher() {
        @Override
        public void beforeTextChanged(CharSequence s, int start, int count, int after) {

        }

        @Override
        public void onTextChanged(CharSequence s, int start, int before, int count) {
            try {
                GoodsProvider.mCashAmount = Double.valueOf(s.toString());
            } catch (NumberFormatException e) {
                GoodsProvider.mCashAmount = 0.0;
                e.printStackTrace();
            }
            checkTotalAmounts();
        }

        @Override
        public void afterTextChanged(Editable s) {

        }
    };

    TextWatcher watchBank = new TextWatcher() {
        @Override
        public void beforeTextChanged(CharSequence s, int start, int count, int after) {

        }

        @Override
        public void onTextChanged(CharSequence s, int start, int before, int count) {
            try {
                GoodsProvider.mBankAmount = Double.valueOf(s.toString());
            } catch (NumberFormatException e) {
                GoodsProvider.mBankAmount = 0.0;
                e.printStackTrace();
            }
            checkTotalAmounts();
        }

        @Override
        public void afterTextChanged(Editable s) {

        }
    };

    TextWatcher watchDept = new TextWatcher() {
        @Override
        public void beforeTextChanged(CharSequence s, int start, int count, int after) {

        }

        @Override
        public void onTextChanged(CharSequence s, int start, int before, int count) {
            try {
                GoodsProvider.mDebtAmount = Double.valueOf(s.toString());
            } catch (NumberFormatException e) {
                GoodsProvider.mDebtAmount = 0.0;
                e.printStackTrace();
            }
            checkTotalAmounts();
        }

        @Override
        public void afterTextChanged(Editable s) {

        }
    };
}
