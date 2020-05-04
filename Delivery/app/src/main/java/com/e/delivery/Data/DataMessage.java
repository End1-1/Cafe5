package com.e.delivery.Data;

import android.os.Parcel;
import android.os.Parcelable;

public class DataMessage implements Parcelable {
    public static int mCounter = 0;
    public int mLocalCounter;
    public int mCommand;
    public String mBuffer;
    public int mResponse;
    public String mSender;
    public String mReceiver;

    public DataMessage(int command, String buffer, String sender, String receiver) {
        mCounter++;
        mLocalCounter = mCounter;
        mResponse = 0;
        mCommand = command;
        mBuffer = buffer;
        mSender = sender;
        mReceiver = receiver;
    }

    public void rotate() {
        String s = mSender;
        mSender = mReceiver;
        mReceiver = s;
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