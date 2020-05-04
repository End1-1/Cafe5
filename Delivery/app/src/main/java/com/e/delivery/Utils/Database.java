package com.e.delivery.Utils;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.SQLException;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

import androidx.annotation.Nullable;

public class Database extends SQLiteOpenHelper {
    public static final int dbversion = 7;
    private ContentValues contentValues;
    private SQLiteDatabase dbLite;
    private Cursor cursor;

    public Database(@Nullable Context context) {
        super(context, "delivery", null, dbversion);
        dbLite = getWritableDatabase();
    }

    @Override
    public void onCreate(SQLiteDatabase db) {
        db.execSQL("create table oh (id integer primary key autoincrement, unid text, upid text, taxcode text, customer text, atotal double, acash double, abank double, adept double, coordinates text)");
        db.execSQL("create table ob (id integer primary key autoincrement, oh integer, goods integer, qty double, price double)");
        db.execSQL("create table partners (taxcode text, taxname text, info text, contact text, phone text, address text)");
        db.execSQL("create table gg (id integer primary key, name text)");
        db.execSQL("create table g (id integer primary key, gg integer, name text, price1 double, price2 double)");
    }

    public void noException(SQLiteDatabase db, String sql) {
        try {
            db.execSQL(sql);
        } catch (SQLException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
        if (oldVersion != newVersion) {
            noException(db, "drop table oh");
            noException(db, "drop table ob");
            noException(db, "drop table partners");
            noException(db, "drop table g");
            noException(db, "drop table gg");
            onCreate(db);
        }
    }

    public ContentValues getContentValues() {
        if (contentValues == null) {
            contentValues = new ContentValues();
        }
        contentValues.clear();
        return contentValues;
    }

    public boolean exec(String sql) {
        try {
            dbLite.execSQL(sql);
        } catch (SQLException e) {
            e.printStackTrace();
            return false;
        }
        return true;
    }

    public Cursor select(String sql) {
        cursor = dbLite.rawQuery(sql, null);
        return cursor;
    }

    public boolean moveToNext() {
        if (cursor == null) {
            return false;
        }
        return cursor.moveToNext();
    }

    public int getInt(String columnName) {
        return cursor.getInt(cursor.getColumnIndex(columnName));
    }

    public String getString(String columnName) {
        return cursor.getString(cursor.getColumnIndex(columnName));
    }

    public boolean insert(String table) {
        try {
            dbLite.insertOrThrow(table, null, contentValues);
            contentValues.clear();
        } catch (SQLException e) {
            e.printStackTrace();
            return false;
        }
        return true;
    }

    public int insertWithId(String table) {
        if (insert(table)) {
            Cursor c = dbLite.rawQuery("select last_insert_rowid()", null);
            if (c.moveToLast()) {
                return c.getInt(0);
            }
        }
        return 0;
    }

    public void beginTransaction() {
        dbLite.beginTransaction();
    }

    public void commitTransactoin() {
        dbLite.setTransactionSuccessful();
    }

    public void endTransaction() {
        dbLite.endTransaction();
    }

    public void close() {
        dbLite.close();
    }
}
