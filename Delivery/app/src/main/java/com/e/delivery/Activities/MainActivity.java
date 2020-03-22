package com.e.delivery.Activities;

import android.Manifest;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.view.View;
import android.view.inputmethod.InputMethodManager;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;
import androidx.core.app.ActivityCompat;

import com.e.delivery.Data.DataMessage;
import com.e.delivery.Data.GoodsProvider;
import com.e.delivery.Data.PartnerProvider;
import com.e.delivery.R;
import com.e.delivery.Services.LocalMessanger;
import com.e.delivery.Services.TempService;
import com.e.delivery.Utils.Config;
import com.e.delivery.Utils.DataSenderCommands;
import com.e.delivery.Utils.EnumView;
import com.e.delivery.Utils.Json;
import com.e.delivery.Utils.ViewAnimator;

public class MainActivity extends ParentActivity {

    final int REQUEST_ACCESS_FINE_LOCATION = 1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        if (Config.getString("username").length() > 0) {
            setEditText(R.id.edUsername, Config.getString("username"));
            setEditText(R.id.edPassword, Config.getString("password"));
        }
        EnumView.setButtonsClickListener(findViewById(R.id.idParent), this);
        Intent intent = new Intent(this, TempService.class);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            startForegroundService(intent);
        } else {
            startService(intent);
        }
        ActivityCompat.requestPermissions(this, new String[] {
                Manifest.permission.ACCESS_FINE_LOCATION,
                Manifest.permission.ACCESS_COARSE_LOCATION }, REQUEST_ACCESS_FINE_LOCATION);
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        switch (requestCode) {
            case REQUEST_ACCESS_FINE_LOCATION:
                if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                } else {
                    AlertDialog.Builder ab = new AlertDialog.Builder(this);
                    ab.setMessage(R.string.YouMustAllowToAccessLocationService);
                    ab.setCancelable(false);
                    ab.setNegativeButton(R.string.Close, new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            finish();
                        }
                    });
                    AlertDialog dlg = ab.create();
                    dlg.show();
                }
                break;
        }
    }

    @Override
    protected void messageHandler(DataMessage m) {
        super.messageHandler(m);
        switch (m.mCommand) {
            case DataSenderCommands.lServiceStarted:
                if (findViewById(R.id.clConfig).getVisibility() == View.VISIBLE) {
                    String session = Config.getString("session_id");
                    if (!session.isEmpty()) {
                        findViewById(R.id.progressBar).setVisibility(View.VISIBLE);
                        findViewById(R.id.tvLoginStatus).setVisibility(View.VISIBLE);
                        findViewById(R.id.btnEnter).setEnabled(false);
                        setTextViewText(R.id.tvLoginStatus, getString(R.string.LoginStatus));
                        Json j = new Json();
                        j.putString("session", session);
                        j.putInt("listofgoods", 1);
                        m = new DataMessage(DataSenderCommands.qLoginWithSession, j.toString(), "A", "S");
                        LocalMessanger.sendMessage(m);
                    }
                }
                break;
            case DataSenderCommands.qLogin:
            case DataSenderCommands.qLoginWithSession:
                Json data = new Json(m.mBuffer);
                if (m.mResponse == DataSenderCommands.rOk) {
                    Config.setString("username", editText(R.id.edUsername));
                    Config.setString("password", editText(R.id.edPassword));
                    Config.setString("session_id", data.getString("session"));
                    Config.setString("fullname", data.getString("lastname") + " " + data.getString("firstname"));
                    setTextViewText(R.id.tvFullname, Config.getString("fullname"));
                    setTextViewText(R.id.tvLoginStatus, getString(R.string.LoginStatusGoodsGroups));
                    Json gg = data.getJsonObject("listofgoodsgroups");
                    Json goodsGroups = gg.getJsonArray("groups");
                    GoodsProvider.initGoodsGroups(goodsGroups.getArray("groups"));
                    setTextViewText(R.id.tvLoginStatus, getString(R.string.LoginStatusGoods));
                    Json goods = gg.getJsonArray("goods");
                    GoodsProvider.initGoods(goods.getArray("goods"));
                    Json partners = gg.getJsonArray("partners");
                    PartnerProvider.initPartners(partners.getArray("partners"));
                    ViewAnimator.animateHeight(findViewById(R.id.clConfig), -1, 0, hideLogin);
                } else {
                    Config.setString("session_id", "");
                    findViewById(R.id.progressBar).setVisibility(View.GONE);
                    findViewById(R.id.btnEnter).setEnabled(true);
                    String msg = String.format("%s\n%s", getString(R.string.LoginStatusFailed), data.getString("msg"));
                    setTextViewText(R.id.tvLoginStatus, msg);
                }
                break;
        }
    }

    @Override
    public void onClick(View view) {
        Intent i;
        super.onClick(view);
        switch (view.getId()) {
            case R.id.btnEnter:
                findViewById(R.id.progressBar).setVisibility(View.VISIBLE);
                findViewById(R.id.tvLoginStatus).setVisibility(View.VISIBLE);
                findViewById(R.id.btnEnter).setEnabled(false);
                setTextViewText(R.id.tvLoginStatus, getString(R.string.LoginStatus));
                Json j = new Json();
                j.putString("username", editText(R.id.edUsername));
                j.putString("password", editText(R.id.edPassword));
                j.putInt("listofgoods", 1);
                DataMessage m = new DataMessage(DataSenderCommands.qLogin, j.toString(), "A", "S");
                LocalMessanger.sendMessage(m);
                break;
            case R.id.btnSettings:
                i = new Intent(this, ConfigActivity.class);
                startActivity(i);
                break;
            case R.id.btnOrder:
                i = new Intent(this, OrderActivity.class);
                startActivity(i);
                break;
            case R.id.btnSales:
                i = new Intent(this, SalesTodayActivity.class);
                startActivity(i);
                break;
            case R.id.btnLogout:
                ViewAnimator.animateHeight(findViewById(R.id.llWorking), -1, 0, showLogin);
                break;
        }
    }

    ViewAnimator.ViewAnimatorEnd hideLogin = new ViewAnimator.ViewAnimatorEnd() {
        @Override
        public void end() {
            InputMethodManager inputMethodManager = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
            inputMethodManager.toggleSoftInput(InputMethodManager.HIDE_IMPLICIT_ONLY, 0);
            ViewAnimator.animateHeight(findViewById(R.id.llWorking), 0, findViewById(R.id.idParent).getMeasuredHeight(), null);
        }
    };

    ViewAnimator.ViewAnimatorEnd showLogin = new ViewAnimator.ViewAnimatorEnd() {
        @Override
        public void end() {
            Config.setString("username", "");
            Config.setString("password", "");
            Config.setString("session_id", "");
            setEditText(R.id.edUsername, Config.getString("username"));
            setEditText(R.id.edPassword, Config.getString("password"));
            findViewById(R.id.progressBar).setVisibility(View.GONE);
            findViewById(R.id.tvLoginStatus).setVisibility(View.GONE);
            findViewById(R.id.edUsername).requestFocus();
            ViewAnimator.animateHeight(findViewById(R.id.clConfig), 0, findViewById(R.id.idParent).getMeasuredHeight(), null);
        }
    };
}
