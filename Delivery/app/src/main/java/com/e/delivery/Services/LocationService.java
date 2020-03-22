package com.e.delivery.Services;

import android.location.Location;
import android.location.LocationListener;
import android.os.Bundle;

import com.e.delivery.Data.DataMessage;
import com.e.delivery.Utils.DataSenderCommands;

import java.util.Calendar;
import java.util.IllegalFormatException;

public class LocationService implements LocationListener {

    public static Location mLastLocation = null;

    @Override
    public void onLocationChanged(Location location) {
        mLastLocation = location;
        DataMessage m = new DataMessage(DataSenderCommands.qLocationChanged, mark(), "S", "S");
        LocalMessanger.sendMessage(m);
    }

    @Override
    public void onStatusChanged(String provider, int status, Bundle extras) {

    }

    @Override
    public void onProviderEnabled(String provider) {

    }

    @Override
    public void onProviderDisabled(String provider) {

    }

    public static String mark() {
        if (mLastLocation == null) {
            return "0|0|0|0";
        }
        try {
            double lat =  mLastLocation.getLatitude();
            double lon = mLastLocation.getLongitude();
            double speed = mLastLocation.getSpeed();
            double time = mLastLocation.getTime();
            if (time < 0.001) {
                time = Calendar.getInstance().getTime().getTime();
            }
            return String.format("%f|%f|%f|%f", lat, lon, speed, time);
        } catch (IllegalFormatException e) {
            e.printStackTrace();
        }
        return "0|0|0|0";
    }
}
