#include "armsoft.h"
#include "database.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>

ArmSoft::ArmSoft(const QJsonObject &data, QObject *parent) :
    QObject(parent),
    fData(data)
{

}

bool ArmSoft::exportToAS(const QString &orderUuid, QString &err)
{
    QSqlDatabase dbas = QSqlDatabase::addDatabase("QODBC", "as3");
    dbas.setDatabaseName(fData["asconnectionstring"].toString());
    dbas.setUserName("sa");
    dbas.setPassword("SaSa111");
    qDebug() << dbas.databaseName() << dbas.userName() << dbas.password();
    if (!dbas.open()) {
        err = dbas.lastError().databaseText();
        return false;
    }
    getIndexes(err, dbas);

    //NOW EXPORT
    Database db;
    if (!db.open(fData["database"].toObject())) {
        err = db.lastDbError();
        return false;
    }


    db[":f_id"] = orderUuid;
    db.exec("select * from o_draft_sale where f_id=:f_id");
    if (db.next() == false) {
        err = QString("%1: %2").arg(tr("Invalid order id")).arg(orderUuid);
        return false;
    }
    QDate docDate = db.date("f_date");
    QString docid = db.uuid();
    int partner = db.integer("f_partner");
    db[":f_asdbid"] = fData["asdbid"].toInt();
    db[":f_table"] = "c_partners";
    db[":f_tableid"] = partner;
    db.exec("select * from as_convert where f_asdbid=:f_asdbid and f_table=:f_table and f_tableid=:f_tableid");
    if (db.next()) {
        partner = db.integer("f_ascode");
        if (partner == 0) {
            err = tr("No partner code exists for ArmSoft");
            return false;
        }
    } else {
        err = tr("No partner code exists for ArmSoft");
        return false;
    }
    db[":f_id"] = partner;
    db.exec("select * from c_partners where f_id=:f_id");
    db.next();
    int pricepolitic = db.integer("f_price_politic");
    QString partnerAddress = db.string("f_address");

    db[":f_header"] = orderUuid;
    db[":f_asdbid"] = fData["asdbid"].toInt();
    db[":f_pp"] = pricepolitic;
    db.exec("SELECT ds.f_goods, a.f_ascode, g.f_name, ds.f_price, ds.f_qty, "
            "if (:f_pp=1, gp.f_price1, gp.f_price2) as f_initprice, 0 as f_discount, "
            "g.f_service, a2.f_ascode as f_asstore "
            "FROM o_draft_sale_body ds "
            "LEFT JOIN c_goods g ON g.f_id=ds.f_goods "
            "LEFT JOIN c_goods_prices gp ON gp.f_goods=g.f_id AND gp.f_currency=1 "
            "left join as_convert a on a.f_tableid=g.f_id and a.f_table='c_goods' and a.f_asdbid=:f_asdbid "
            "left join as_convert a2 on a2.f_tableid=ds.f_store and a2.f_table='c_storages' and a2.f_asdbid=:f_asdbid "
            "where ds.f_header=:f_header and ds.f_state=1 ");
    double total = 0;
    QList<QMap<QString, QVariant> > items;
    while (db.next()) {
        if (db.string("f_ascode").isEmpty()) {
            err += QString("%1 %2. \r\n").arg(tr("No goods code exists for"), db.string("f_name"));
        }
        QMap<QString, QVariant> tmp;
        tmp["f_ascode"] = db.string("f_ascode");
        tmp["f_goods"] = db.integer("f_goods");
        tmp["f_name"] = db.string("f_name");
        tmp["f_qty"] = db.doubleValue("f_qty");
        tmp["f_initprice"] = db.doubleValue("f_initprice");
        tmp["f_price"] = db.doubleValue("f_price");
        tmp["f_discount"] = ((db.doubleValue("f_initprice") - db.doubleValue("f_price")) * 100) / db.doubleValue("f_initprice");
        tmp["f_service"] = db.integer("f_service");
        tmp["f_store"] = db.string("f_asstore");
        items.append(tmp);
        total += db.doubleValue("f_price") * db.doubleValue("f_discount");
    }
    if (!err.isEmpty()) {
        return false;
    }
    if (items.count() == 0) {
        err = tr("Empty invoice");
        return false;
    }

    QSqlQuery q(dbas);
    dbas.transaction();
    if (q.prepare("insert into DOCUMENTS ("
                  "fISN, fDOCTYPE, fDOCSTATE, fDATE, fORDERNUM, fDOCNUM, fCUR, fSUMM, fCOMMENT, fBODY, fPARTNAME, "
                  "fUSERID, fPARTID, fCRPARTID, fMTID, fEXPTYPE, fINVN, fENTRYSTATE, "
                  "fEMPLIDRESPIN, fEMPLIDRESPOUT, fVATTYPE, fSPEC, fCHANGEDATE,fEXTBODY,fETLSTATE) VALUES ("
                  ":fISN, :fDOCTYPE, :fDOCSTATE, '" + docDate.toString("yyyy-MM-dd") + "', "
                  ":fORDERNUM, :fDOCNUM, :fCUR, :fSUMM, :fCOMMENT, :fBODY, :fPARTNAME, "
                  ":fUSERID, :fPARTID, :fCRPARTID, :fMTID, '', :fINVN, :fENTRYSTATE, "
                  ":fEMPLIDRESPIN, :fEMPLIDRESPOUT, :fVATTYPE, :fSPEC, current_timestamp,'', '')") == false) {
        dbas.rollback();
        err = q.lastError().databaseText();
        return false;
    }
    q.bindValue(":fISN", docid);
    q.bindValue(":fDOCTYPE", fData["doctype"].toInt());
    q.bindValue(":fDOCSTATE", 1);
    q.bindValue(":fORDERNUM", "");
    q.bindValue(":fDOCNUM", "");
    q.bindValue(":fCUR", "AMD");
    q.bindValue(":fSUMM", total);
    q.bindValue(":fCOMMENT", partnerAddress);
    q.bindValue(":fBODY", QString("\r\nPREPAYMENTACC:5231\r\nVATACC:5243\r\nSUMMVAT:%2\r\nBUYERACC:2211\r\n"
                "CUREXCHNG:1.0000\r\nCOURSECOUNT:1.0000\r\nBUYCHACCPOST:Գլխավոր հաշվապահ \r\nMAXROWID:%1\r\n")
            .arg(items.count())
            .arg(total));
    q.bindValue(":fPARTNAME", partnersMap[partner]["fcaption"]); // set to kamar
    q.bindValue(":fUSERID", 0);
    q.bindValue(":fPARTID", partnersMap[partner]["fpartid"]);
    q.bindValue(":fCRPARTID", -1);
    q.bindValue(":fMTID", -1);
    q.bindValue(":fINVN", "");
    q.bindValue(":fENTRYSTATE", 0);
    q.bindValue(":fEMPLIDRESPIN", -1);
    q.bindValue(":fEMPLIDRESPOUT", -1);
    q.bindValue(":fVATTYPE", fData["doctype"].toInt() == 20 ? "1" : "2");
    q.bindValue(":fSPEC", "                    00"); // <--- Tax receipt id
//        if (card > 0.001) {
//            dbas[":fBODY"] = dbas[":fBODY"].toString() +
//                    QString("BANKACQUIRINGACCOUNT:%1\r\nSUMMPLCARD:%2\r\n")
//                    .arg(bankacc)
//                    .arg(float_str(card, 2).replace(",", "").replace(" ", ""));
//        }
    if (!q.exec()) {
        dbas.rollback();
        err = q.lastError().databaseText();
        return false;
    }

    int rowid = 1;
    for (QList<QMap<QString, QVariant> >::const_iterator bi = items.constBegin(); bi != items.constEnd(); bi++) {
        q.prepare("insert into MTINVOICELIST (fISN, fROWNUM, fITEMTYPE, fITEMID, fITEMNAME, "
                    "fUNITBRIEF, fSTORAGE, fQUANTITY, fINITPRICE, fDISCOUNT, fPRICE, fSUMMA, fSUMMA1, "
                    "fENVFEEPERCENT, fENVFEESUMMA, fVAT, fEXPMETHOD, fACCEXPENSE, fACCINCOME, fPARTYMETHOD, fROWID) "
                    "VALUES (:fISN, :fROWNUM, :fITEMTYPE, :fITEMID, :fITEMNAME, "
                    ":fUNITBRIEF, :fSTORAGE, :fQUANTITY, :fINITPRICE, :fDISCOUNT, :fPRICE, :fSUMMA, :fSUMMA1, "
                    ":fENVFEEPERCENT, :fENVFEESUMMA, :fVAT, :fEXPMETHOD, :fACCEXPENSE, :fACCINCOME, :fPARTYMETHOD, :fROWID)");
        q.bindValue(":fISN", docid);
        q.bindValue(":fROWNUM", rowid);
        q.bindValue(":fITEMTYPE", (*bi)["f_service"].toInt() == 1 ? 2 : 1);
        q.bindValue(":fITEMID", (*bi)["f_service"].toInt() == 1 ? servicesMap[(*bi)["f_ascode"].toString()]["fservid"] : goodsMap[(*bi)["f_ascode"].toString()]["fmtid"]);
        q.bindValue(":fITEMNAME", (*bi)["f_name"]);
        q.bindValue(":fUNITBRIEF", unitsMap[servicesMap[(*bi)["f_ascode"].toString()]["funit"].toString()]);
        q.bindValue(":fSTORAGE", (*bi)["f_store"]);
        q.bindValue(":fQUANTITY", 1);
        q.bindValue(":fINITPRICE", (*bi)["f_initprice"]);
        q.bindValue(":fDISCOUNT", (*bi)["f_discount"]);
        q.bindValue(":fPRICE", (*bi)["f_price"]);
        q.bindValue(":fSUMMA", (*bi)["f_qty"].toDouble() * (*bi)["f_price"].toDouble());
        q.bindValue(":fSUMMA1", 0);
        q.bindValue(":fENVFEEPERCENT", 0);
        q.bindValue(":fENVFEESUMMA", 0);
        q.bindValue(":fVAT", 1);
        q.bindValue(":fEXPMETHOD", 1);
        q.bindValue(":fACCEXPENSE", (*bi)["f_service"].toInt() == 1 ? fData["lesexpenseacc"].toString() : fData["lemexpenseacc"].toString());
        q.bindValue(":fACCINCOME", (*bi)["f_service"].toInt() == 1 ? fData["lesincomeacc"].toString() : fData["lemincomeacc"].toString());
        q.bindValue(":fPARTYMETHOD", 0);
        q.bindValue(":fROWID", rowid++);
        if (q.exec() == false) {
            dbas.rollback();
            err = q.lastError().databaseText()
                          + "<br>"
                          + orderUuid
                          + "<br>"
                          + (*bi)["f_name"].toString();
            return false;
        }
    }
    dbas.commit();

    return true;
}

bool ArmSoft::getIndexes(QString &err, QSqlDatabase &dbas)
{
    QSqlQuery q(dbas);
    q.setForwardOnly(true);

    q.exec("select fMTCODE, fMTID, fCAPTION, fUNIT from MATERIALS");
    QSqlRecord r = q.record();
    QStringList fields;
    for (int i = 0; i < r.count(); i++) {
        fields.append(r.fieldName(i).toLower());
    }
    while (q.next()) {
        QMap<QString, QVariant> temp;
        recordToMap(temp, q, fields);
        goodsMap[q.value(0).toString()] = temp;
    }
    fields.clear();

    q.exec("select fSERVID, fSERVCODE, fCAPTION, fFULLCAPTION, fUNIT from SERVICES");
    r = q.record();
    for (int i = 0; i < r.count(); i++) {
        fields.append(r.fieldName(i).toLower());
    }
    while (q.next()) {
        QMap<QString, QVariant> temp;
        recordToMap(temp, q, fields);
        servicesMap[q.value(1).toString()] = temp;
    }
    fields.clear();

    q.exec("select fCODE, fBRIEF from QNTUNIT");
    while (q.next()) {
        QMap<QString, QVariant> temp;
        recordToMap(temp, q, fields);
        unitsMap[q.value(0).toString()] = temp;
    }
    fields.clear();

    q.exec("select fPARTID, fPARTCODE, fCAPTION, fFULLCAPTION from PARTNERS");
    r = q.record();
    for (int i = 0; i < r.count(); i++) {
        fields.append(r.fieldName(i).toLower());
    }
    while (q.next()) {
        QMap<QString, QVariant> temp;
        recordToMap(temp, q, fields);
        partnersMap[q.value(1).toInt()] = temp;
    }
    return true;
}

void ArmSoft::recordToMap(QMap<QString, QVariant> &m, QSqlQuery &q, QStringList &fields)
{
    for (int i = 0; i < fields.count(); i++) {
        m[fields[i]] = q.value(i);
    }
}
