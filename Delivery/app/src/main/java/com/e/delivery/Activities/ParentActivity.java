package com.e.delivery.Activities;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.widget.EditText;
import android.widget.TextView;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.FragmentTransaction;
import androidx.localbroadcastmanager.content.LocalBroadcastManager;

import com.e.delivery.Data.DataMessage;
import com.e.delivery.Fragments.ParentFragment;
import com.e.delivery.Services.TempService;
import com.e.delivery.Utils.LocaleHelper;

public class ParentActivity extends AppCompatActivity implements View.OnClickListener {
    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getSupportActionBar().hide();
        LocaleHelper.setLocale(this, "hy");
    }

    @Override
    protected void onStart() {
        super.onStart();
        LocalBroadcastManager.getInstance(this).registerReceiver(mMessageReceiver, new IntentFilter("A"));
    }

    @Override
    protected void onStop() {
        super.onStop();
        LocalBroadcastManager.getInstance(this).unregisterReceiver(mMessageReceiver);
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

    public void replaceFragment(ParentFragment fr, int containerId) {
        FragmentTransaction fragmentTransaction = getSupportFragmentManager().beginTransaction();
        fragmentTransaction.replace(containerId, fr, fr.tag());
        fragmentTransaction.commit();
    }

    protected void messageHandler(DataMessage m) {

    }

    protected void sendMessage(DataMessage m) {
        Intent i = new Intent("S");
        i.putExtra("datamessage", m);
        LocalBroadcastManager.getInstance(this).sendBroadcast(i);
    }

    private BroadcastReceiver mMessageReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            DataMessage m = intent.getParcelableExtra("datamessage");
            messageHandler(m);
        }
    };
}
