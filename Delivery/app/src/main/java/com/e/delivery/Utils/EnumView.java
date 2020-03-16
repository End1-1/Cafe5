package com.e.delivery.Utils;

import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.LinearLayout;

import androidx.constraintlayout.widget.ConstraintLayout;

public class EnumView {

    public static void setButtonsClickListener (View view, View.OnClickListener clickListener){
        ViewGroup vg = (ViewGroup) view;
        for(int i = 0; i < vg.getChildCount(); i++){
            View v = vg.getChildAt(i);
            if(v instanceof Button){
                v.setOnClickListener(clickListener);
            } else if (v instanceof ImageButton) {
                v.setOnClickListener(clickListener);
            } else if (v instanceof ConstraintLayout) {
                setButtonsClickListener(v, clickListener);
            } else if (v instanceof LinearLayout) {
                setButtonsClickListener(v, clickListener);
            }
        }
    }
}
