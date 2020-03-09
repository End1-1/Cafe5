package com.e.delivery.Activities;

import android.os.Bundle;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.TextView;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.constraintlayout.widget.ConstraintLayout;

import com.e.delivery.Utils.LocaleHelper;

public class ParentActivity extends AppCompatActivity implements View.OnClickListener {
    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getSupportActionBar().hide();
        LocaleHelper.setLocale(this, "hy");
    }

    protected void setButtonsClickListener (ViewGroup vg){
        for(int i = 0; i < vg.getChildCount(); i++){
            View v = vg.getChildAt(i);
            if(v instanceof Button){
                v.setOnClickListener(this);
            } else if (v instanceof ImageButton) {
                v.setOnClickListener(this);
            } else if (v instanceof ConstraintLayout) {
                setButtonsClickListener((ViewGroup) v);
            }
        }
    }

    @Override
    public void onClick(View view) {

    }

    public void setEditText(int id, String text) {
        EditText ed = findViewById(id);
        ed.setText(text);
    }

    public String editText(int id) {
        EditText ed = findViewById(id);
        return ed.getText().toString();
    }

    public void setTextViewText(int id, String text) {
        TextView tv = findViewById(id);
        tv.setText(text);
    }
}
