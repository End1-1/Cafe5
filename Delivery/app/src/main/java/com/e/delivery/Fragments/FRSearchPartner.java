package com.e.delivery.Fragments;

import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import android.text.Editable;
import android.text.TextWatcher;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.e.delivery.Activities.OrderActivity;
import com.e.delivery.Data.Partner;
import com.e.delivery.Data.PartnerProvider;
import com.e.delivery.R;
import com.e.delivery.Utils.EnumView;

public class FRSearchPartner extends ParentFragment {

    EditText edSearch;
    PartnersAdapter mPartnersAdapter;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View v = inflater.inflate(R.layout.fragment_search_partner, container, false);
        edSearch = v.findViewById(R.id.edSearch);
        edSearch.addTextChangedListener(watcher);
        EnumView.setButtonsClickListener(v, this);
        RecyclerView rv = v.findViewById(R.id.rv);
        rv.setLayoutManager(new LinearLayoutManager(v.getContext()));
        mPartnersAdapter = new PartnersAdapter(rv);
        rv.setAdapter(mPartnersAdapter);
        return v;
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btnClear:
                edSearch.setText("");
                break;
            case R.id.btnReady:
                Partner partner = null;
                if (mPartnersAdapter.mAdapterPosition > -1) {
                    partner = PartnerProvider.mProxyPartners.get(mPartnersAdapter.mAdapterPosition);
                }
                ((OrderActivity) mActivity).setPartner(partner);
                break;
        }
    }

    TextWatcher watcher = new TextWatcher() {
        int lastSize = 0;

        @Override
        public void beforeTextChanged(CharSequence s, int start, int count, int after) {

        }

        @Override
        public void onTextChanged(CharSequence s, int start, int before, int count) {
            if (lastSize < s.toString().length()) {
                PartnerProvider.searchInProxy(s.toString());
            } else {
                PartnerProvider.searchInAll(s.toString());
            }
            lastSize = s.toString().length();
            mPartnersAdapter.notifyDataSetChanged();
        }

        @Override
        public void afterTextChanged(Editable s) {

        }
    };

    public class PartnersAdapter extends RecyclerView.Adapter<RecyclerView.ViewHolder> implements View.OnClickListener {

        RecyclerView recyclerView;
        int mAdapterPosition = -1;

        @Override
        public void onClick(View v) {
            mAdapterPosition = recyclerView.getChildLayoutPosition(v);
            notifyDataSetChanged();
        }

        class VH extends RecyclerView.ViewHolder {

            LinearLayout llParent;
            TextView tvTaxCode;
            TextView tvTaxName;
            TextView tvName;
            TextView tvAddress;
            TextView tvPhone;

            public VH(@NonNull View itemView) {
                super(itemView);
                itemView.setOnClickListener(PartnersAdapter.this);
                llParent = itemView.findViewById(R.id.idParent);
                tvTaxCode = itemView.findViewById(R.id.tvTaxCode);
                tvTaxName = itemView.findViewById(R.id.tvTaxName);
                tvName = itemView.findViewById(R.id.tvName);
                tvAddress = itemView.findViewById(R.id.tvAddress);
                tvPhone = itemView.findViewById(R.id.tvPhone);
            }

            public void onBind(int position) {
                Partner p = PartnerProvider.mProxyPartners.get(position);
                tvTaxCode.setText(p.mTaxCode);
                tvTaxName.setText(p.mTaxName);
                tvName.setText(p.mMarketName);
                tvAddress.setText(p.mAddress);
                tvPhone.setText(p.mPhone);
                if (position == mAdapterPosition) {
                    llParent.setBackgroundColor(getContext().getResources().getColor(R.color.colorRVSelected));
                } else {
                    llParent.setBackgroundColor(getContext().getResources().getColor(R.color.colorWhite));
                }
            }
        }

        public PartnersAdapter(RecyclerView rv) {
            recyclerView = rv;
        }

        @NonNull
        @Override
        public RecyclerView.ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
            return new VH(FRSearchPartner.this.getLayoutInflater().inflate(R.layout.item_search_partner, parent, false));
        }

        @Override
        public void onBindViewHolder(@NonNull RecyclerView.ViewHolder holder, int position) {
            VH vh = (VH) holder;
            vh.onBind(position);
        }

        @Override
        public int getItemCount() {
            return PartnerProvider.mProxyPartners.size();
        }
    }
}
