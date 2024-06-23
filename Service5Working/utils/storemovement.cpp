#include "storemovement.h"
#include <QDebug>

bool writeOutput(const QString &docId, QString &err, Database &db)
{
    double amount = 0;
    int storeOut = 0;
    int storeIn = 0;
    int reason = 0;
    int docType = 0;
    QString complectId;
    int complectCode = 0;
    double complectQty = 0;
    QDate date;
    db[":f_id"] = docId;
    db.exec("select h.f_type, h.f_date, hh.f_storein, hh.f_storeout, hh.f_complectation, hh.f_complectationqty"
            " from a_header h inner join a_header_store hh on hh.f_id=h.f_id where h.f_id=:f_id");
    if (db.next()) {
        date = db.date("f_date");
        storeIn = db.integer("f_storein");
        storeOut = db.integer("f_storeout");
        docType = db.integer("f_type");
        complectCode = db.integer("f_complectation");
        complectQty = db.doubleValue("f_complectationqty");
    } else {
        err += QObject::tr("Invalid document id") + "<br>";
    }
    if (complectCode > 0) {
        db[":f_document"] = docId;
        db[":f_type"] = 1;
        db.exec("select f_id from a_store_draft where f_document=:f_document and f_type=:f_type");
        if (db.next()) {
            complectId = db.string(0);
        }
    }
    QStringList recID;
    QStringList baseID;
    QStringList goodsID;
    QStringList goodsName;
    QList<double> qtyList;
    QList<double> priceList;
    QList<double> totalList;
    db[":f_document"] = docId;
    db.exec("select s.f_id, s.f_goods, s.f_store, s.f_qty, s.f_price, s.f_total, "
            "concat(g.f_name, if(g.f_scancode is null, '', concat(' ', g.f_scancode))) as f_name, "
            "s.f_base, s.f_reason, s.f_comment "
            "from a_store_draft s inner join c_goods g on g.f_id=s.f_goods "
            "where f_document=:f_document and f_type=-1");
    while (db.next()) {
        recID.append(db.string("f_id"));
        baseID.append(db.string("f_base"));
        goodsID.append(db.string("f_goods"));
        goodsName.append(db.string("f_name"));
        reason = db.integer("f_reason");
        qtyList.append(db.doubleValue("f_qty"));
        priceList.append(db.doubleValue("f_price"));
        totalList.append(db.doubleValue("f_total"));
    }
    QList<QMap<QString, QVariant> > storeData;
    db[":f_store"] = storeOut;
    db[":f_date"] = date;
    if (!db.exec(QString("select s.f_id, s.f_goods, sum(s.f_qty*s.f_type) as f_qty, s.f_price, s.f_total*s.f_type, "
                         "s.f_document, s.f_base, d.f_date, sd.f_comment "
                         "from a_store s "
                         "inner join a_header d on d.f_id=s.f_document "
                         "left join a_store_draft sd on s.f_draft=sd.f_id "
                         "where s.f_goods in (%1) and s.f_store=:f_store and d.f_date<=:f_date "
                         "group by s.f_base "
                         "having sum(s.f_qty*s.f_type)>0 "
                         "order by d.f_date "
                         "for update ").arg(goodsID.join(",")))) {
        err = db.lastDbError() + "<br>";
        return false;
    }
    while (db.next()) {
        QMap<QString, QVariant> v;
        for (int i = 0; i < db.columnCount(); i++) {
            v[db.columnName(i)] = db.value(i);
        }
        storeData.append(v);
    }
    QList<QMap<QString, QVariant> > queries;
    QList<QMap<QString, QVariant> > draftcomment;
    for (int i = 0; i < goodsID.count(); i++) {
        double qty = qtyList.at(i);
        totalList[i] = 0;
        for (int j = 0; j < storeData.count(); j++) {
            if (storeData.at(j)["f_goods"].toInt() == goodsID.at(i).toInt()) {
                if (storeData.at(j)["f_qty"].toDouble() > 0) {
                    if (storeData.at(j)["f_qty"].toDouble() >= qty) {
                        storeData[j]["f_qty"] = storeData.at(j)["f_qty"].toDouble() - qty;
                        QMap<QString, QVariant> newrec;
                        newrec[":f_document"] = docId;
                        newrec[":f_store"] = storeOut;
                        newrec[":f_type"] = -1;
                        newrec[":f_goods"] = goodsID.at(i).toInt();
                        newrec[":f_qty"] = qty;
                        newrec[":f_price"] = storeData.at(j)["f_price"];
                        newrec[":f_total"] = storeData.at(j)["f_price"].toDouble() * qty;
                        newrec[":f_base"] = storeData.at(j)["f_base"].toString();
                        newrec[":f_basedoc"] = storeData.at(j)["f_document"];
                        newrec[":f_reason"] = reason;
                        newrec[":f_draft"] = recID.at(i);
                        if (!storeData.at(j)["f_comment"].toString().isEmpty()) {
                            draftcomment.append({{recID.at(i), storeData.at(j)["f_comment"].toString()}});
                        }
                        queries << newrec;
                        amount += storeData.at(j)["f_price"].toDouble() * qty;
                        totalList[i] = totalList[i] + (storeData.at(j)["f_price"].toDouble() * qty);
                        priceList[i] = totalList[i] / qtyList.at(i);
                        qty = 0;
                    } else {
                        QMap<QString, QVariant> newrec;
                        newrec[":f_document"] = docId;
                        newrec[":f_store"] = storeOut;
                        newrec[":f_type"] = -1;
                        newrec[":f_goods"] = goodsID.at(i).toInt();
                        newrec[":f_qty"] = storeData.at(j)["f_qty"].toDouble();
                        newrec[":f_price"] = storeData.at(j)["f_price"].toDouble();
                        newrec[":f_total"] = storeData.at(j)["f_price"].toDouble() * storeData.at(j)["f_qty"].toDouble();
                        newrec[":f_base"] = storeData.at(j)["f_base"];
                        newrec[":f_basedoc"] = storeData.at(j)["f_document"];
                        newrec[":f_reason"] = reason;
                        newrec[":f_draft"] = recID.at(i);
                        if (!storeData.at(j)["f_comment"].toString().isEmpty()) {
                            draftcomment.append({{recID.at(i), storeData.at(j)["f_comment"].toString()}});
                        }
                        queries << newrec;
                        totalList[i] = totalList[i] + (storeData.at(j)["f_qty"].toDouble() * storeData.at(j)["f_price"].toDouble());
                        priceList[i] = totalList[i] / qtyList.at(i);
                        amount += storeData.at(j)["f_qty"].toDouble() * storeData.at(j)["f_price"].toDouble();
                        qty -= storeData.at(j)["f_qty"].toDouble();
                        storeData[j]["f_qty"] = 0.0;
                    }
                }
            }
            if (qty < 0.00001) {
                break;
            }
        }
        if (qty > 0.00001) {
            if (err.isEmpty()) {
                err += QObject::tr("Not enough materials in the store") + "<br>";
            }
            err += QString("%1 - %2 (%3)").arg(goodsName.at(i)).arg(qty).arg(QString::number(storeOut)) + "<br>";
        }
    }
    if (err.isEmpty()) {
        QStringList outId;
        for (QList<QMap<QString, QVariant> >::const_iterator it = queries.constBegin(); it != queries.constEnd(); it++) {
            QString newId = db.uuid();
            outId << newId;
            db.setBindValues( *it);
            db[":f_id"] = newId;
            db.insert("a_store");
        }
        db[":f_document"] = docId;
        db.exec("select f_type, sum(f_total) as f_total from a_store where f_document=:f_document group by 1");
        if (db.next()) {
            db[":f_amount"] = db.doubleValue("f_total");
            db.update("a_header", "f_id", docId);
        }
        if (storeIn > 0) {
            switch (docType) {
                case 3:
                    foreach (QString recid, outId) {
                        db[":f_id"] = recid;
                        db.exec("select * from a_store where f_id=:f_id");
                        db.next();
                        db.setBindValues(db.getBindValues());
                        QString mid = db.uuid();
                        db[":f_base"] = mid;
                        db[":f_basedoc"] = db[":f_id"];
                        db[":f_id"] = mid;
                        db[":f_document"] = docId;
                        db[":f_type"] = 1;
                        db[":f_store"] = storeIn;
                        if (!db.insert("a_store")) {
                            err = db.lastDbError();
                            return false;
                        }
                    }
                    break;
                case 6:
                    db[":f_header"] = docId;
                    db.exec("select sum(f_amount) from a_complectation_additions where f_header=:f_header");
                    db.next();
                    amount += db.doubleValue(0);
                    QString id = db.uuid();
                    db[":f_id"] = id;
                    db[":f_document"] = docId;
                    db[":f_store"] = storeIn;
                    db[":f_type"] = 1;
                    db[":f_goods"] = complectCode;
                    db[":f_qty"] = complectQty;
                    db[":f_price"] = amount / complectQty;
                    db[":f_total"] = amount;
                    db[":f_base"] = outId.at(0);
                    db[":f_basedoc"] = docId;
                    db[":f_reason"] = reason;
                    db[":f_draft"] = complectId;
                    if (!db.insert("a_store")) {
                        err = db.lastDbError();
                        return false;
                    }
                    break;
            }
        }
        for (int i = 0; i < recID.count(); i++) {
            db[":f_price"] = priceList.at(i);
            db[":f_total"] = totalList.at(i);
            db.update("a_store_draft", "f_id", recID.at(i));
            if (!baseID.at(i).isEmpty()) {
                db[":f_price"] = priceList.at(i);
                db[":f_total"] = totalList.at(i);
                db.update("a_store_draft", "f_id", baseID.at(i));
            }
        }
        if (docType == 6) {
            db[":f_type"] = 1;
            db[":f_document"] = docId;
            db[":f_price"] = amount / complectQty;
            db[":f_total"] = amount;
            db.exec("update a_store_draft set f_price=:f_price, f_total=:f_total where f_type=:f_type and f_document=:f_document");
        }
        for (const QMap<QString, QVariant> &m : draftcomment) {
            for (QMap<QString, QVariant>::const_iterator it = m.constBegin(); it != m.constEnd(); it++) {
                db[":f_comment"] = it.value();
                db.update("a_store_draft", "f_id", it.key());
            }
        }
    }
    return err.isEmpty();
}

bool writeMovement(const QString &docId, int goods, double qty, Database &db)
{
    Q_UNUSED(qty);
    Q_UNUSED(goods);
    db.startTransaction();
    QString err;
    if (!writeOutput(docId, err, db)) {
        return false;
    }
    return true;
}
