package com.e.delivery.Fragments;

import android.os.Bundle;

import androidx.fragment.app.Fragment;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.e.delivery.R;

public class FROrderBill extends Fragment {

    public static String mTag = "FROrderBill";

    public static FROrderBill newInstance() {
        FROrderBill fragment = new FROrderBill();
        return fragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_order_bill, container, false);
    }
}
