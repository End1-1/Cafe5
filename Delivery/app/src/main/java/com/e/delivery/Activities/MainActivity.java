package com.e.delivery.Activities;

import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.view.View;
import android.view.ViewGroup;

import com.e.delivery.Data.GoodsProvider;
import com.e.delivery.R;
import com.e.delivery.Services.TempService;
import com.e.delivery.Utils.Config;
import com.e.delivery.Utils.DataSender;
import com.e.delivery.Utils.DataSenderCommands;
import com.e.delivery.Utils.EnumView;
import com.e.delivery.Utils.Json;
import com.e.delivery.Utils.ViewAnimator;

import org.json.JSONObject;

public class MainActivity extends ParentActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        EnumView.setButtonsClickListener(findViewById(R.id.idParent), this);
        Intent intent = new Intent(this, TempService.class);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            startForegroundService(intent);
        } else {
            startService(intent);
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (findViewById(R.id.clConfig).getVisibility() == View.VISIBLE) {
            String session = Config.getString(this, "session_id");
            if (!session.isEmpty()) {
                findViewById(R.id.progressBar).setVisibility(View.VISIBLE);
                findViewById(R.id.tvLoginStatus).setVisibility(View.VISIBLE);
                findViewById(R.id.btnEnter).setEnabled(false);
                setTextViewText(R.id.tvLoginStatus, getString(R.string.LoginStatus));
                Json j = new Json();
                j.putString("session", session);
                j.putInt("listofgoods", 1);
                DataSender ds = new DataSender(j.toString(), DataSenderCommands.qLoginWithSession, dsLogin);
                ds.execute();
            }
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
                DataSender ds = new DataSender(j.toString(), DataSenderCommands.qLogin, dsLogin);
                ds.execute();
                break;
            case R.id.btnSettings:
                i = new Intent(this, ConfigActivity.class);
                startActivity(i);
                break;
            case R.id.btnOrder:
                i = new Intent(this, OrderActivity.class);
                startActivity(i);
                break;
        }
    }

    ViewAnimator.ViewAnimatorEnd hideLogin = new ViewAnimator.ViewAnimatorEnd() {
        @Override
        public void end() {
            ViewAnimator.animateHeight(findViewById(R.id.llWorking), 0, findViewById(R.id.idParent).getMeasuredHeight(), null);
        }
    };

    DataSender.DataSenderCallback dsLogin = new DataSender.DataSenderCallback() {
        @Override
        public void finish(int result, Json data) {
            if (result == DataSenderCommands.rOk) {
                Config.setString(MainActivity.this,"session_id", data.getString("session"));
                setTextViewText(R.id.tvLoginStatus, getString(R.string.LoginStatusGoodsGroups));
                Json gg = data.getJsonObject("listofgoodsgroups");
                Json goodsGroups = gg.getJsonArray("groups");
                GoodsProvider.initGoodsGroups(goodsGroups.getArray("groups"));
                setTextViewText(R.id.tvLoginStatus, getString(R.string.LoginStatusGoods));
                Json goods = gg.getJsonArray("goods");
                GoodsProvider.initGoods(goods.getArray("goods"));
                ViewAnimator.animateHeight(findViewById(R.id.clConfig), -1, 0, hideLogin);
            } else {
                Config.setString(MainActivity.this,"session_id", "");
                findViewById(R.id.progressBar).setVisibility(View.GONE);
                findViewById(R.id.btnEnter).setEnabled(true);
                String msg = String.format("%s\n%s", getString(R.string.LoginStatusFailed), data.getString("msg"));
                setTextViewText(R.id.tvLoginStatus, msg);
            }
        }
    };
}
