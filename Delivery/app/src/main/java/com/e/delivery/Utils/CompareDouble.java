package com.e.delivery.Utils;

public class CompareDouble {

    public static boolean eq(Double v1, Double v2) {
        Double v = v1 - v2;
        if (v > -0.0001 && v < 0.0001) {
            return true;
        }
        return false;
    }
}
