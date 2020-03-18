package com.e.delivery.Fragments;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;

import com.e.delivery.Activities.OrderActivity;
import com.e.delivery.Data.Partner;
import com.e.delivery.R;
import com.e.delivery.Utils.EnumView;

public class FRCustomer extends ParentFragment {

    Partner mPartner;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View v = inflater.inflate(R.layout.fragment_costumer, container, false);
        EnumView.setButtonsClickListener(v, this);
        mPartner = ((OrderActivity) mActivity).getPartner();
        boolean e = mPartner == null;
        v.findViewById(R.id.edTaxNumber).setEnabled(e);
        v.findViewById(R.id.edTaxName).setEnabled(e);
        v.findViewById(R.id.edMarketName).setEnabled(e);
        v.findViewById(R.id.edContact).setEnabled(e);
        v.findViewById(R.id.edAddress).setEnabled(e);
        v.findViewById(R.id.edPhone).setEnabled(e);
        return v;
    }

    @Override
    public void onResume() {
        super.onResume();
        if (mPartner != null) {
            ((EditText) getView().findViewById(R.id.edTaxNumber)).setText(mPartner.mTaxCode);
            ((EditText) getView().findViewById(R.id.edTaxName)).setText(mPartner.mTaxName);
            ((EditText) getView().findViewById(R.id.edMarketName)).setText(mPartner.mMarketName);
            ((EditText) getView().findViewById(R.id.edContact)).setText(mPartner.mContact);
            ((EditText) getView().findViewById(R.id.edAddress)).setText(mPartner.mAddress);
            ((EditText) getView().findViewById(R.id.edPhone)).setText(mPartner.mPhone);
        }
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.btnSearch:
                ((OrderActivity) mActivity).searchPartner();
                break;
        }
    }
}
