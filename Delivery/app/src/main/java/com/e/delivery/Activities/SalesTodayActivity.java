package com.e.delivery.Activities;

import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.e.delivery.R;
import com.e.delivery.Utils.Database;

import java.util.LinkedList;
import java.util.List;

public class SalesTodayActivity extends ParentActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_sales_today);
        RecyclerView rv = findViewById(R.id.rv);
        rv.setLayoutManager(new LinearLayoutManager(this));
        SalesAdapter salesAdapter = new SalesAdapter(this);
        rv.setAdapter(salesAdapter);
        ((TextView) findViewById(R.id.tvTotalAmount)).setText(String.format("%.0f", salesAdapter.mTotal));
    }

    public class SalesAdapter extends RecyclerView.Adapter<RecyclerView.ViewHolder> {

        List<OH> mOh;
        Double mTotal = 0.0;

        class OH {
            int mId;
            String mTaxcode;
            String mName;
            Double mAmount;
        }

        Database db;

        public SalesAdapter (Context context) {
            db = new Database(context);
            mOh = new LinkedList<>();
            refresh();
        }

        public void refresh() {
            mOh.clear();
            Cursor c = db.select("select * from oh");
            if (c.moveToFirst()) {
                do {
                    OH oh = new OH();
                    oh.mId = c.getInt(c.getColumnIndex("id"));
                    oh.mName = c.getString(c.getColumnIndex("customer"));
                    oh.mTaxcode = c.getString(c.getColumnIndex("taxcode"));
                    oh.mAmount = c.getDouble(c.getColumnIndex("atotal"));
                    mTotal += oh.mAmount;
                    mOh.add(oh);
                } while (c.moveToNext());
            }
            db.close();
            notifyDataSetChanged();
        }

        class VH extends RecyclerView.ViewHolder implements View.OnLongClickListener {

            OH mVhOh;
            TextView tvName;
            TextView tvAmount;

            public VH(@NonNull View itemView) {
                super(itemView);
                itemView.setOnLongClickListener(this);
                tvName = itemView.findViewById(R.id.tvName);
                tvAmount = itemView.findViewById(R.id.tvAmount);
            }

            public void onBind(int position) {
                mVhOh = mOh.get(position);
                tvName.setText(mVhOh.mName);
                tvAmount.setText(String.format("%.0f", mVhOh.mAmount));
            }

            @Override
            public boolean onLongClick(View v) {
                Intent i = new Intent(SalesTodayActivity.this, OrderActivity.class);
                i.putExtra("order", mVhOh.mId);
                startActivity(i);
                return false;
            }
        }

        @NonNull
        @Override
        public RecyclerView.ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
            return new VH(getLayoutInflater().inflate(R.layout.item_sales_today, parent, false));
        }

        @Override
        public void onBindViewHolder(@NonNull RecyclerView.ViewHolder holder, int position) {
            VH vh = (VH) holder;
            vh.onBind(position);
        }

        @Override
        public int getItemCount() {
            return mOh.size();
        }
    }
}
