#include "rwshop.h"
#include "database.h"
#include "goodsreserve.h"
#include "logwriter.h"
#include "commandline.h"
#include "chatmessage.h"
#include "tablerecord.h"
#include "sqlqueries.h"
#include <QSettings>
#include <QDir>
#include <QFile>
#include <QLocale>
#include <QJsonDocument>
#include <QJsonObject>

#define op_check_quantity 3
#define op_reserve_goods 4
#define op_create_document 5
#define op_data_list 6
#define op_image 7

#define list_storages 1

#define float_str(value, f) QLocale().toString(value, 'f', f).remove(QRegExp("(?!\\d[\\.\\,][1-9]+)0+$")).remove(QRegExp("[\\.\\,]$"))

QMap<QString, QString> grMap;

QString gr(const QString &v) {
    if (grMap.isEmpty()) {
        QFile grFile(":/gr.txt");
        if (grFile.open(QIODevice::ReadOnly)) {
            QString grLine;
            do {
                grLine = QString::fromUtf8(grFile.readLine());
                if (grLine.isEmpty() == false) {
                    QStringList grsplit = grLine.split('=');
                    grMap[grsplit.at(0).toLower()] = grsplit.at(1);
                }
            } while (grLine.isEmpty());
            grFile.close();
        }
    }
    if (grMap.contains(v.toLower())) {
        return grMap[v.toLower()];
    } else {
        return "gr_" + v;
    }
}

int putData(RawMessage &rm, const QVariant &value){
    switch (value.type()) {
    case QVariant::Int:
        return rm.putUInt(value.toInt());
    case QVariant::Double:
        return rm.putDouble(value.toDouble());
    case QVariant::String:
        return rm.putString(value.toString());
    default:
        return rm.putString(value.toString());
    }
}

bool checkQuantity(quint8 op, RawMessage &rm, Database &db, const QByteArray &in) {
    QString scancode;
    rm.readString(scancode, in);

    db[":f_scancode"] = scancode;
    db.exec("select f_id from c_goods where f_scancode=:f_scancode");
    if (db.next() == false) {
        rm.putUByte(2);
        rm.putString(gr("Invalid scancode"));
        return false;
    }
    db[":f_date"] = QDate::currentDate();
    db[":f_goods"] = db.integer("f_id");
    db[":f_reservestate"] = GR_RESERVED;
    db.exec("select ss.f_name as f_storename,g.f_name as f_goodsname,g.f_scancode, "
            "u.f_name as f_unit,g.f_saleprice,g.f_saleprice2, sum(s.f_qty*s.f_type) as f_qty, "
            "coalesce(rs.f_qty, 0) as f_reserveqty, s.f_store, s.f_goods "
            "from a_store s "
            "inner join c_storages ss on ss.f_id=s.f_store "
            "inner join c_goods g on g.f_id=s.f_goods "
            "inner join c_units u on u.f_id=g.f_unit "
            "inner join a_header h on h.f_id=s.f_document  "
            "left join (select f_goods, f_store, sum(f_qty) as f_qty "
                "from a_store_reserve "
                "where f_state=:f_reservestate and f_goods=:f_goods group by 1, 2) "
                "rs on rs.f_goods=s.f_goods and rs.f_store=s.f_store "
            "where  h.f_date<=:f_date and s.f_goods=:f_goods "
            "group by ss.f_name,g.f_name,u.f_name,g.f_saleprice,g.f_saleprice2 "
            "having sum(s.f_qty*s.f_type) > 0");

    quint8 columnCount = db.columnCount();
    quint32 rowCount = db.rowCount();
    rm.putUByte(op);
    rm.putUByte(columnCount);
    rm.putUInt(rowCount);

    //columns
    rm.putUByte(3);
    rm.putUByte(3);
    rm.putUByte(3);
    rm.putUByte(3);
    rm.putUByte(2);
    rm.putUByte(2);
    rm.putUByte(2);
    rm.putUByte(2);
    rm.putUByte(1);
    rm.putUByte(1);

    //default widths
    rm.putUShort(122);
    rm.putUShort(0);
    rm.putUShort(0);
    rm.putUShort(0);
    rm.putUShort(0);
    rm.putUShort(0);
    rm.putUShort(0);
    rm.putUShort(0);
    rm.putUShort(0);
    rm.putUShort(123);

    //titles
    rm.putString(gr("Storage"));
    rm.putString(gr("Goods name"));
    rm.putString(gr("Scancode"));
    rm.putString(gr("Unit"));
    rm.putString(gr("Retail price"));
    rm.putString(gr("Whosale price"));
    rm.putString(gr("Quantity"));
    rm.putString(gr("Reserved"));
    rm.putString(gr("Store code"));
    rm.putString(gr("Goods code"));

    //totals
    rm.putUByte(0);
    rm.putUByte(0);
    rm.putUByte(0);
    rm.putUByte(0);
    rm.putUByte(0);
    rm.putUByte(0);
    rm.putUByte(1);
    rm.putUByte(1);
    rm.putUByte(0);
    rm.putUByte(0);

    quint32 *matrix = new quint32[columnCount * rowCount];
    int headeroffcet = 3 + (sizeof(quint32) * 3) + sizeof(quint16);
    int row = 0;
    while (db.next()) {
        matrix[(row * columnCount) + 0] = putData(rm, db.value("f_storename")) - headeroffcet;
        matrix[(row * columnCount) + 1] = putData(rm, db.value("f_goodsname")) - headeroffcet;
        matrix[(row * columnCount) + 2] = putData(rm, db.value("f_scancode"))  - headeroffcet;
        matrix[(row * columnCount) + 3] = putData(rm, db.value("f_unit")) - headeroffcet;
        matrix[(row * columnCount) + 4] = putData(rm, db.value("f_saleprice")) - headeroffcet;
        matrix[(row * columnCount) + 5] = putData(rm, db.value("f_saleprice2")) - headeroffcet;
        matrix[(row * columnCount) + 6] = putData(rm, db.value("f_qty")) - headeroffcet;
        matrix[(row * columnCount) + 7] = putData(rm, db.value("f_reserveqty")) - headeroffcet;
        matrix[(row * columnCount) + 8] = putData(rm, db.value("f_store")) - headeroffcet;
        matrix[(row * columnCount) + 9] = putData(rm, db.value("f_goods")) - headeroffcet;
        qDebug() << matrix[(row * columnCount) + 0] << matrix[(row * columnCount) + 1] << matrix[(row * columnCount) + 2] << matrix[(row * columnCount) + 3] << matrix[(row * columnCount) + 4] << matrix[(row * columnCount) + 5] <<matrix[(row * columnCount) + 6] << matrix[(row * columnCount) + 7] << matrix[(row * columnCount) + 8] << matrix[(row * columnCount) + 9];
        row++;
    }
    rm.putBytes(reinterpret_cast<const char*>(matrix), columnCount  * rowCount * sizeof(quint32));
    delete [] matrix;
    return true;
}

bool reserveGoods(quint8 op, RawMessage &rm, Database &db, const QByteArray &in) {
    QString dateFor, msg, goodsName, scancode, unit;
    quint32 goods, storeSrc, storeDst;
    double qty;

    rm.readString(dateFor, in);
    rm.readUInt(goods, in);
    rm.readUInt(storeDst, in);
    rm.readUInt(storeSrc, in);
    rm.readDouble(qty, in);
    rm.readString(msg, in);
    rm.readString(goodsName, in);
    rm.readString(scancode, in);
    rm.readString(unit, in);

    LogWriter::write(LogWriterLevel::verbose, "",
                     QString("rwshop::reserveGoods: %1,%2,%3,%4,%6,%7,%8")
                     .arg(dateFor, QString::number(goods), QString::number(storeDst), QString::number(storeSrc),
                          float_str(qty, 2), msg, goodsName, scancode, unit));

    db[":f_store"] = storeDst;
    db[":f_goods"] = goods;
    db[":f_qty"] = qty;
    db[":f_message"] = msg;
    db[":f_source"] = storeSrc;
    db[":f_enddate"] = QDate::fromString(dateFor, "dd/MM/yyyy");
    db[":f_date"] = QDate::currentDate();
    db[":f_time"] = QTime::currentTime();
    db[":f_state"] = GR_RESERVED;

    int reply = 0;
    if (db.insert("a_store_reserve", reply) == false) {
        rm.putUByte(2);
        rm.putString(db.lastDbError());
        return false;
    }

    db[":f_qty"] = qty;
    db[":f_goods"] = goods;
    db[":f_store"] = storeDst;
    db.exec("update a_store_sale set f_qtyreserve=f_qtyreserve+:f_qty where f_store=:f_store and f_goods=:f_goods");

    QJsonObject jo;
    jo["action"] = MSG_GOODS_RESERVE;
    jo["goods"] = (int) goods;
    jo["qty"] = qty;
    jo["goodsname"] = goodsName;
    jo["scancode"] = scancode;
    jo["unit"] = unit;
    jo["usermessage"] = msg;
    jo["enddate"] = dateFor;
    QJsonDocument jdoc(jo);

    CommandLine cl;
    QString configFile;
    cl.value("--config", configFile);
    Database dbserver;
    if (dbserver.open(configFile) == false ){
        return false;
    }

    dbserver[":fid"] = storeSrc;
    dbserver.exec("select fuser from users_store where fid=:fid");
    if (dbserver.next()) {
        storeSrc = dbserver.integer("fuser");
    }
    dbserver[":fid"] = storeDst;
    dbserver.exec("select fuser from users_store where fid=:fid");
    if (dbserver.next()) {
        storeDst = dbserver.integer("fuser");
    }

    dbserver[":fdateserver"] = QDateTime::currentDateTime();
    dbserver[":fstate"] = 1;
    dbserver[":fsender"] = storeSrc;
    dbserver[":freceiver"] = storeDst;
    dbserver[":fmessage"] = jdoc.toJson();
    int msgid = 0;
    dbserver.insert("users_chat", msgid);

    rm.putUByte(op);
    rm.putUInt(reply);
    return true;
}

bool document(quint8 op, RawMessage &rm, Database &db, const QByteArray &in) {
    quint8 doctype, action;
    rm.readUByte(doctype, in);
    rm.readUByte(action, in);
    quint32 storein, storeout;
    rm.readUInt(storein, in);
    rm.readUInt(storeout, in);

    TableRecord ah("a_header", db);
    TableRecord ahs("a_header_store", db);

    db.startTransaction();
    QString docUuid = db.uuid();
    ah[":f_id"] = docUuid;
    ah[":f_userid"] = 0;
    ah[":f_state"] = 2; //DOC_STATE_DRAFT;
    ah[":f_type"] = doctype;
    ah[":f_operator"] = 1;
    ah[":f_date"] = QDate::currentDate();
    ah[":f_createdate"] = QDate::currentDate();
    ah[":f_createtime"] = QTime::currentTime();
    ah[":f_partner"] = 0;
    ah[":f_amount"] = 0;
    ah[":f_comment"] = "";
    ah[":f_payment"] = 0;
    ah[":f_paid"] = 0;
    if (ah.insert() == false) {
        db.rollback();
        rm.putUByte(2);
        rm.putString(db.lastDbError());
        return false;
    }

    ahs[":f_id"] = docUuid;
    ahs[":f_useraccept"] = 0;
    ahs[":f_userpass"] = 0;
    ahs[":f_invoice"] = "";
    ahs[":f_invoicedate"] = QDate::fromString("01/01/1990", "dd/MM/yyyy");
    ahs[":f_storein"] = storein;
    ahs[":f_storeout"] = storeout;
    ahs[":f_baseonsale"] = 0;
    ahs[":f_cashuuid"] = "";
    ahs[":f_complectation"] = 0;
    ahs[":f_complectationqty"] = 0;
    if (ahs.insert() == false) {
        db.rollback();
        rm.putUByte(2);
        rm.putString(db.lastDbError());
        return false;
    }
    db.commit();

    rm.putUByte(op);
    rm.putString(docUuid);
    return true;
}

bool datalist(quint8 op, RawMessage &rm, Database &db, const QByteArray &in) {
    quint8 list;
    rm.readUByte(list, in);
    switch (list) {
    case list_storages:
        db.exec("select f_id, f_name from c_storages order by f_name");
        break;
    default:
        rm.putUByte(2);
        rm.putString("Unknown list number");
        return false;
    }
    rm.putUByte(op);
    rm.putUByte(list);
    rm.putUInt(db.rowCount());
    while (db.next()) {
        for (int i = 0; i < db.columnCount(); i++) {
            putData(rm, db.value(i));
        }
    }
    return true;
}

bool image(quint8 op, RawMessage &rm, Database &db, const QByteArray &in) {
    QString scancode;
    rm.readString(scancode, in);

    db[":f_scancode"] = scancode;
    db.exec("select f_id, f_name from c_goods where f_scancode=:f_scancode");
    if (db.next() == false) {
        rm.putUByte(2);
        rm.putString("Incorrect scancode");
        return true;
    }


    quint8 img = 0;
    db[":f_id"] = db.integer("f_id");
    QString name = db.string("f_name");
    db.exec("select f_data from c_goods_images where f_id=:f_id");
    if (db.next()) {
        img = 1;
    }

    rm.putUByte(op);
    rm.putUByte(img);
    rm.putString(scancode);
    rm.putString(name);
    if (img == 1) {
        rm.putBytes(db.value("f_data").toByteArray().data(), db.value("f_data").toByteArray().size());
    }
    return true;
}

bool dllfunc(const QByteArray &in, RawMessage &rm)
{
    QString dbname;
    rm.readString(dbname, in);
    quint8 op;
    rm.readUByte(op, in);

    CommandLine cl;
    QString configFile;
    cl.value("--config", configFile);
    QSettings s(configFile, QSettings::IniFormat);
    LogWriter::write(LogWriterLevel::verbose, rm.property("session").toString(),QString("rwshop. config: %1").arg(configFile));
    Database db;
    if (db.open(s.value("db/host").toString(), s.value("db/schema").toString(), s.value("db/username").toString(), s.value("db/password").toString()) == false) {
        rm.putUByte(2);
        rm.putString(db.lastDbError());
        LogWriter::write(LogWriterLevel::errors, rm.property("session").toString(), db.lastDbError());
        return false;
    }
    db[":fname"] = dbname;
    db.exec("select * from system_databases where fname=:fname");
    if (db.next() == false) {
        rm.putUByte(2);
        rm.putString("Unknown database name");
        return false;
    }
    Database dbw;
    if (dbw.open(db.string("fhost"), db.string("fschema"), db.string("fusername"), db.string("fpassword")) == false) {
        rm.putUByte(2);
        rm.putString(dbw.lastDbError());
        return false;
    }

    switch (op) {
    case op_check_quantity:
        return checkQuantity(op, rm, dbw, in);
    case op_reserve_goods:
        return reserveGoods(op, rm, dbw, in);
    case op_create_document:
        return document(op, rm, dbw, in);
    case op_data_list:
        return datalist(op, rm, dbw, in);
    case op_image:
        return image(op, rm, dbw, in);
    }
    return false;
}
