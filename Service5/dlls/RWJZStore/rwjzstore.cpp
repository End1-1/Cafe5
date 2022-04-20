#include "rwjzstore.h"
#include "database.h"

class Config {
public:
    QMap<int, QMap<QString, QString> > fConnections;
    QMap<int, QString> fStorages;
    QMap<int, QMap<int, QString> > fAsStorageMap;

    Config() {
        init();
    }

    void addConnection(int id, const QString &name, const QString &host, const QString &schema, const QString &username, const QString &password)
    {
        QMap<QString, QString> c;
        c["name"] = name;
        c["host"] = host;
        c["schema"] = schema;
        c["username"] = username;
        c["password"] = password;
        fConnections[id] = c;
    }

    void init() {
        if (fConnections.isEmpty()) {
            addConnection(2, QString::fromUtf8("Օպերա"), "10.10.12.1", "cafe4", "SYSDBA", "Inter_OneStep");
            addConnection(3, QString::fromUtf8("Թումանյան"), "10.10.5.1", "cafe4", "SYSDBA", "Inter_OneStep");
            addConnection(4, QString::fromUtf8("Կոմիտաս"), "10.10.11.1", "cafe4", "SYSDBA", "Inter_OneStep");
            addConnection(6, QString::fromUtf8("Աբովյան"), "10.10.13.1", "cafe4", "SYSDBA", "Inter_OneStep");
            addConnection(7, QString::fromUtf8("Արմենիա"), "10.10.7.1", "cafe4", "SYSDBA", "Inter_OneStep");
            addConnection(8, QString::fromUtf8("Երևան Մոլլ"), "10.10.14.1", "cafe4", "SYSDBA", "Inter_OneStep");

            fStorages[2] = QString::fromUtf8("Բար");
            fStorages[3] = QString::fromUtf8("Խոհանոց");
            fStorages[4] = QString::fromUtf8("Սառնարան");

            fAsStorageMap[2][2] = "022";
            fAsStorageMap[2][3] = "023";
            fAsStorageMap[2][4] = "025";
            fAsStorageMap[3][2] = "031";
            fAsStorageMap[3][3] = "032";
            fAsStorageMap[3][4] = "033";
            fAsStorageMap[4][2] = "041";
            fAsStorageMap[4][3] = "042";
            fAsStorageMap[4][4] = "043";
            fAsStorageMap[6][2] = "061";
            fAsStorageMap[6][3] = "062";
            fAsStorageMap[6][4] = "063";
            fAsStorageMap[7][2] = "071";
            fAsStorageMap[8][2] = "081";
            fAsStorageMap[8][3] = "082";
            fAsStorageMap[8][4] = "083";
        }
    }
};

int putData(RawMessage &rm, const QVariant &value){
    qDebug() << value;
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

QString gr(const QString &v) {
    return v;
}

bool dllfunc(const QByteArray &in, RawMessage &rm)
{
    quint16 year;
    quint8 month;
    quint8 cafe;
    quint8 store;
    rm.readUShort(year, in);
    rm.readUByte(month, in);
    rm.readUByte(cafe, in);
    rm.readUByte(store, in);

    Config c;

    QMap<QString, QString> &con = c.fConnections[cafe];
    Database db("QIBASE");
    if (!db.open(con["host"], con["schema"], con["username"], con["password"])) {
        rm.putUByte(2);
        rm.putString(db.lastDbError());
        return false;
    }
    QMap<int, QHash<QString, QVariant> > goods;
    db.exec("select f.id, t.name as typename, f.name as foodname "
            "from food_names f "
            "left join food_groups t on t.id=f.group_id "
            "order by f.name ");
    while (db.next()) {
        QHash<QString, QVariant> r;
        for (int i = 0; i < db.columnCount(); i++) {
            r[db.columnName(i).toLower()] = db.value(i);
        }
        goods[db.integerValue("id")] = r;
    }
    QDate d1 = QDate::fromString(QString("%1/%2/%3").arg(1, 2, 10, QChar('0')).arg(month, 2, 10, QChar('0')).arg(year), "dd/MM/yyyy");
    QDate d2 = QDate::fromString(QString("%1/%2/%3").arg(d1.daysInMonth(), 2, 10, QChar('0')).arg(month, 2, 10, QChar('0')).arg(year), "dd/MM/yyyy");
    db[":date"] = d1.addDays(-1);
    db[":store_id"] = store;
    db[":action_id"] = 7;
    if (!db.exec("select sd.goods_id, sum(sd.qty) as qty, sum(sd.amount) as amount "
                                 "from st_documents st, st_draft sd "
                                 "where st.id=sd.doc_id "
                                 "and st.doc_date = :date and st.store_input=:store_id "
                                 "and st.action_id=:action_id "
                                 "group by 1")) {
        rm.putUByte(2);
        rm.putString(db.lastDbError());
        return false;
    }
    while (db.next()) {
        QHash<QString, QVariant> &j = goods[db.integerValue("goods_id")];
        j["qty_till"] = db.doubleValue("qty");
        j["amount_till"] = db.doubleValue("amount");
    }
    Database mssqldb("QODBC3");
    if (!mssqldb.open("10.1.0.4,1433", "Driver={SQL Server Native Client 11.0};Server=10.1.0.4,1433;DATABASE=Jazzve", "sa", "SaSa111")) {
        rm.putUByte(2);
        rm.putString(mssqldb.lastDbError());
        return false;
    }
    mssqldb[":store"] = c.fAsStorageMap[cafe][store];
    mssqldb[":date1"] = d1;
    mssqldb[":date2"] = d2;
    if (!mssqldb.exec("select cast(t.fMTCODE as integer) as food, m.fDBCR as sign, sum(m.fQTY) as qty, sum(m.fCOSTSUMM) as amount "
                "from Jazzve.dbo.MTHI m, Jazzve.dbo.MATERIALS t , Jazzve.dbo.DOCUMENTS d "
                "where m.fMTID=t.fMTID and d.fISN=m.fBASE and d.fDOCTYPE in (6, 7, 8, 17) "
                "and d.fDATE between :date1 and :date2 and m.fSTORAGE=:store "
                "and (t.fMTCODE NOT LIKE '1-%' and t.fMTCODE NOT LIKE '2-%') "
                "group by cast(t.fMTCODE as integer), m.fDBCR "
                "order by 1")) {
        rm.putUByte(2);
        rm.putString(mssqldb.lastDbError());
        return false;
    }
    QStringList foodNotInDb;
    while (mssqldb.next()) {
        if (!goods.contains(mssqldb.integerValue("food"))) {
            if (!foodNotInDb.contains(mssqldb.stringValue("food"))) {
                foodNotInDb.append(mssqldb.stringValue("food"));
            }
            continue;
        }
        QHash<QString, QVariant> &j = goods[mssqldb.integerValue("food")];
        j["qty_in"] = j["qty_in"].toDouble() + (mssqldb.doubleValue("qty") * (mssqldb.integerValue("sign") == 0 ? -1 : 1));
        j["amount_in"] = j["amount_in"].toDouble() + (mssqldb.doubleValue("amount") * (mssqldb.integerValue("sign") == 0 ? -1 : 1));
    }
    if (foodNotInDb.count() > 0) {
        rm.putUByte(2);
        rm.putString(QString("Goods not in database: %1").arg(foodNotInDb.join(",")));
        return false;
    }
    db[":store"] = store;
    db[":date1"] = d1;
    db[":date2"] = d2;
    if (!db.exec("select r.goods_id, sum(r.qty*d.qty) as qty, 0 as AMOUNT "
            "from o_dishes d  "
            "left join me_recipes r on d.dish_id=r.dish_id "
            "left join o_order o on o.id=d.order_id "
            "where o.date_cash between :date1 and :date2 "
            "and (o.state_id=2 or o.state_id=1) and d.state_id=1 "
            "and r.goods_id is not null "
            "and d.store_id=:store "
            "group by 1")) {
        rm.putUByte(2);
        rm.putString(db.lastDbError());
        return false;
    }
    while (db.next()) {
        QHash<QString, QVariant> &j = goods[db.integerValue("goods_id")];
        j["qty_out"] = db.doubleValue("qty");
        j["amount_out"] = db.doubleValue("amount");
    }


    //OUT2
    db[":store"] = store;
    db[":date1"] = d1;
    db[":date2"] = d2;
    if (!db.exec("select fn.id as goods_id, sum(sdd.qty) as qty,sum(sdd.amount) as amount "
                 "from st_documents sd "
                 "left join st_actions sa on sd.action_id=sa.id "
                 "left join st_storages s2 on s2.id=sd.store_output "
                 "left join st_draft sdd on sdd.doc_id=sd.id "
                 "left join food_names fn on fn.id=sdd.goods_id "
                 "where sd.doc_date between :date1 and :date2 and sa.id in (2) and s2.id=:store "
                 "group by 1 ")) {
        rm.putUByte(2);
        rm.putString(db.lastDbError());
        return false;
    }
    while (db.next()) {
        QHash<QString, QVariant> &j = goods[db.integerValue("goods_id")];
        j["qty_out2"] = db.doubleValue("qty");
        j["amount_out2"] = db.doubleValue("amount");
    }
    QList<int> removeList;
    for (QMap<int, QHash<QString, QVariant> >::iterator it = goods.begin(); it != goods.end(); it++) {
        if (it.value()["qty_till"].toDouble() < 0.0001
                && it.value()["qty_in"].toDouble() < 0.0001
                && it.value()["qty_out"].toDouble() < 0.0001
                && it.value()["qty_out2"].toDouble() < 0.0001) {
            removeList.append(it.key());
        }
    }
    for (auto i: removeList) {
        goods.remove(i);
    }

    /*temp &*/
//    while (goods.count() > 1) {
//        goods.remove(goods.begin().key());
//    }

    quint32 rowCount = goods.count();
    rm.putUByte(3); // answer : 3 = ok
    rm.putUByte(8); //column count
    rm.putUInt(rowCount); //row count

    //column types
    rm.putUByte(1);
    rm.putUByte(3);
    rm.putUByte(3);
    rm.putUByte(2);
    rm.putUByte(2);
    rm.putUByte(2);
    rm.putUByte(2);
    rm.putUByte(2);

    //default widths
    rm.putUShort(80);
    rm.putUShort(100);
    rm.putUShort(300);
    rm.putUShort(80);
    rm.putUShort(80);
    rm.putUShort(80);
    rm.putUShort(80);
    rm.putUShort(80);

    //header
    rm.putString(gr("Goods code"));
    rm.putString(gr("Group name"));
    rm.putString(gr("Goods name"));
    rm.putString(gr("Till"));
    rm.putString(gr("Input"));
    rm.putString(gr("Sale"));
    rm.putString(gr("Output"));
    rm.putString(gr("Remain"));

    //columns that need totals
    rm.putUByte(0);
    rm.putUByte(0);
    rm.putUByte(0);
    rm.putUByte(1);
    rm.putUByte(1);
    rm.putUByte(1);
    rm.putUByte(1);
    rm.putUByte(1);

    quint32 *matrix = new quint32[8 * rowCount];
    int headeroffcet = 3 + (sizeof(quint32) * 3) + sizeof(quint16);

    int row = 0;
    for (QMap<int, QHash<QString, QVariant> >::iterator it = goods.begin(); it != goods.end(); it++) {
        if (!it.value()["qty_till"].isValid()) {
            it.value()["qty_till"] = 0.0;
        }
        if (!it.value()["qty_in"].isValid()) {
            it.value()["qty_in"] = 0.0;
        }
        if (!it.value()["qty_out"].isValid()) {
            it.value()["qty_out"] = 0.0;
        }
        if (!it.value()["qty_out2"].isValid()) {
            it.value()["qty_out2"] = 0.0;
        }
        matrix[(row * 8) + 0] = putData(rm, it.value()["id"]) - headeroffcet;
        matrix[(row * 8) + 1] = putData(rm, it.value()["typename"]) - headeroffcet;
        matrix[(row * 8) + 2] = putData(rm, it.value()["foodname"])  - headeroffcet;
        matrix[(row * 8) + 3] = putData(rm, it.value()["qty_till"]) - headeroffcet;
        matrix[(row * 8) + 4] = putData(rm, it.value()["qty_in"]) - headeroffcet;
        matrix[(row * 8) + 5] = putData(rm, it.value()["qty_out"]) - headeroffcet;
        matrix[(row * 8) + 6] = putData(rm, it.value()["qty_out2"]) - headeroffcet;
        matrix[(row * 8) + 7] = putData(rm, it.value()["qty_till"].toDouble()
                + it.value()["qty_in"].toDouble()
                - it.value()["qty_out"].toDouble()
                - it.value()["qty_out2"].toDouble()) - headeroffcet;
        row++;
    }
    rm.putBytes(reinterpret_cast<const char*>(matrix), 8 * rowCount * sizeof(quint32));
    delete [] matrix;

    return true;
}
