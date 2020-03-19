package com.e.delivery.Data;

import android.os.Parcel;
import android.os.Parcelable;

public class DataMessage implements Parcelable {
    public int mCommand;
    public String mBuffer;
    public int mResponse;

    public DataMessage(int command, String buffer) {
        mCommand = command;
        mBuffer = buffer;
    }

    protected DataMessage(Parcel in) {
        mCommand = in.readInt();
        mBuffer = in.readString();
        mResponse = in.readInt();
    }

    public static final Creator<DataMessage> CREATOR = new Creator<DataMessage>() {
        @Override
        public DataMessage createFromParcel(Parcel in) {
            return new DataMessage(in);
        }

        @Override
        public DataMessage[] newArray(int size) {
            return new DataMessage[size];
        }
    };

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(mCommand);
        dest.writeString(mBuffer);
        dest.writeInt(mResponse);
    }
}