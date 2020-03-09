package com.e.delivery.Activities;

import android.os.Bundle;

import com.e.delivery.R;
import com.e.delivery.Utils.Config;

public class ConfigActivity extends ParentActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_config);
        setEditText(R.id.edServerAddress, Config.mServerIP);
        setEditText(R.id.edServerPort, Integer.toString(Config.mServerPort));
    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();
        Config.mServerIP = editText(R.id.edServerAddress);
        Config.mServerPort = Integer.valueOf(editText(R.id.edServerPort));
        Config.setString(this,"server_ip", Config.mServerIP);
        Config.setInt(this, "server_port", Config.mServerPort);
    }
}
