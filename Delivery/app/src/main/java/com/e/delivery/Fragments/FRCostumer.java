package com.e.delivery.Fragments;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.e.delivery.R;

public class FRCostumer extends ParentFragment {

    public static String mTag = "FRCostumer";

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_costumer, container, false);
    }
}
