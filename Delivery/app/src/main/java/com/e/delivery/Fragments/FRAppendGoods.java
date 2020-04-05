package com.e.delivery.Fragments;

import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.e.delivery.Activities.OrderActivity;
import com.e.delivery.Data.Goods;
import com.e.delivery.Data.GoodsProvider;
import com.e.delivery.R;
import com.e.delivery.Utils.EnumView;

public class FRAppendGoods extends FRParentOrder {

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View v = inflater.inflate(R.layout.fragment_append_goods, container, false);
        EnumView.setButtonsClickListener(v, this);
        RecyclerView rv = v.findViewById(R.id.rvGoods);
        rv.setLayoutManager(new LinearLayoutManager(getContext()));
        rv.setAdapter(new AddGoodsAdapter());
        return v;
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.btnReady:
                ((OrderActivity) mActivity).readyGoods();
                break;
        }
    }

    @Override
    public void disableEdit(boolean v) {
        super.disableEdit(v);
        if (v) {
            ((OrderActivity) mActivity).cancelReadyGoods();
        }
    }

    public class AddGoodsAdapter extends RecyclerView.Adapter<RecyclerView.ViewHolder> {

        public class VH extends RecyclerView.ViewHolder {
            TextView tvGoodsName;
            TextView tvPrice;
            TextView tvStoreQty;
            EditText edQty;
            LinearLayout llBackground;
            QtyTextWatcher mQtyTextWatcher;
            public VH(@NonNull View view, QtyTextWatcher tw) {
                super(view);
                tvGoodsName = view.findViewById(R.id.tvName);
                tvPrice = view.findViewById(R.id.tvPrice);
                tvStoreQty = view.findViewById(R.id.tvQtyStore);
                edQty = view.findViewById(R.id.edQty);
                mQtyTextWatcher = tw;
                edQty.addTextChangedListener(tw);
                llBackground = view.findViewById(R.id.llBackground);
            }

            void onBind(int position) {
                mQtyTextWatcher.setPosition(position);
                Goods g = GoodsProvider.mProxyGoods.get(position);
                tvGoodsName.setText(g.mName);
                tvPrice.setText(String.format("%.0f", g.mRetailPrice));
                tvStoreQty.setText(String.format("%.0f", 0.0));
                if (g.mSelectedQty > 0.001) {
                    edQty.setText(String.format("%.0f", g.mSelectedQty));
                } else {
                    edQty.setText("");
                }
                int color = position % 2 == 0 ? R.color.colorLightGray : R.color.colorWhite;
                llBackground.setBackgroundColor(getResources().getColor(color));
            }
        }

        @NonNull
        @Override
        public RecyclerView.ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
            return new VH(getLayoutInflater().inflate(R.layout.item_add_goods, parent, false), new QtyTextWatcher());
        }

        @Override
        public void onBindViewHolder(@NonNull RecyclerView.ViewHolder holder, int position) {
            VH vh = (VH) holder;
            vh.onBind(position);
        }

        @Override
        public int getItemCount() {
            return GoodsProvider.proxyGoodsCount();
        }

        public class QtyTextWatcher implements TextWatcher {
            private int mPosition;

            public void setPosition(int pos) {
                mPosition = pos;
            }

            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {

            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {
                Double d;
                try {
                    d = Double.valueOf(s.toString());
                } catch (NumberFormatException e) {
                    return;
                }
                Goods g = GoodsProvider.mProxyGoods.get(mPosition);
                g.mSelectedQty = d;
            }

            @Override
            public void afterTextChanged(Editable s) {

            }
        }
    }
}
