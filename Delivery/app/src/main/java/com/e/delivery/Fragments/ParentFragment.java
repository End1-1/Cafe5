package com.e.delivery.Fragments;

import android.content.Context;
import android.os.Bundle;
import android.view.View;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;

import com.e.delivery.Activities.ParentActivity;

public class ParentFragment extends Fragment implements View.OnClickListener {

    protected ParentActivity mActivity;

    public ParentFragment() {

    }

    public String tag() {
        return getClass().getName();
    }

    @Override
    public void onAttach(@NonNull Context context) {
        super.onAttach(context);
        if (context instanceof ParentActivity) {
            mActivity = (ParentActivity) context;
        }
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public void onClick(View v) {

    }
}
