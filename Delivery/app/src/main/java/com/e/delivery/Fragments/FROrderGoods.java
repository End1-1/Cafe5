package com.e.delivery.Fragments;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.e.delivery.Data.Goods;
import com.e.delivery.Data.GoodsProvider;
import com.e.delivery.R;
import com.e.delivery.Utils.EnumView;

public class FROrderGoods extends FRParentOrder {

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View v = inflater.inflate(R.layout.fragment_order_goods, container, false);
        EnumView.setButtonsClickListener(v, this);
        RecyclerView rv = v.findViewById(R.id.rv);
        rv.setLayoutManager(new LinearLayoutManager(getContext()));
        rv.setAdapter(new ReadyGoodsAdapter());
        return v;
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.btnAppendGoods:
                appendGoods();
                break;
        }
    }

    @Override
    public void disableEdit(boolean v) {
        super.disableEdit(v);
        int visible = v ? View.GONE : View.VISIBLE;
        getView().findViewById(R.id.btnAppendGoods).setVisibility(visible);
    }

    void appendGoods() {
        mActivity.replaceFragment(new FRAppendGoods(), R.id.fragment);
    }

    public class ReadyGoodsAdapter extends RecyclerView.Adapter<RecyclerView.ViewHolder> {

        public class VH extends RecyclerView.ViewHolder {
            TextView tvGoodsName;
            TextView tvPrice;
            TextView tvStoreQty;
            EditText edQty;
            LinearLayout llBackground;
            public VH(@NonNull View view) {
                super(view);
                tvGoodsName = view.findViewById(R.id.tvName);
                tvPrice = view.findViewById(R.id.tvPrice);
                tvStoreQty = view.findViewById(R.id.tvQtyStore);
                edQty = view.findViewById(R.id.edQty);
                llBackground = view.findViewById(R.id.llBackground);
            }

            void onBind(int position) {
                Goods g = GoodsProvider.mReadyGoods.get(position);
                tvGoodsName.setText(g.mName);
                tvPrice.setText(String.format("%.0f", g.mRetailPrice));
                tvStoreQty.setText(String.format("%.2f", 0.0));
                String s = String.format("%.2f", g.mSelectedQty);
                edQty.setText(s);
                edQty.setSelection(s.length() - 1, 1);
                int color = position % 2 == 0 ? R.color.colorLightGray : R.color.colorWhite;
                llBackground.setBackgroundColor(getResources().getColor(color));
            }
        }

        @NonNull
        @Override
        public RecyclerView.ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
            return new VH(getLayoutInflater().inflate(R.layout.item_ready_goods, parent, false));
        }

        @Override
        public void onBindViewHolder(@NonNull RecyclerView.ViewHolder holder, int position) {
            VH vh = (VH) holder;
            vh.onBind(position);
        }

        @Override
        public int getItemCount() {
            return GoodsProvider.readyGoodsCount();
        }
    }
}
