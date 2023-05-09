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
    Database dbas ("QODBC");
    if (!dbas.open("", fData["asconnectionstring"].toString(), "sa", "SaSa111")) {
        err = dbas.lastDbError();
        return false;
    }
    getIndexes(err, dbas.fSqlDatabase);

    //NOW EXPORT
    Database db;
    if (!db.open(fData["database"].toObject())) {
        err = db.lastDbError();
        return false;
    }


    db[":f_id"] = orderUuid;
    db.exec("select ds.f_saletype, oh.* from o_header oh left join o_draft_sale ds on oh.f_id=ds.f_id where oh.f_id=:f_id");
    if (db.next() == false) {
        err = QString("%1: %2").arg(tr("Invalid order id")).arg(orderUuid);
        return false;
    }
    QDate docDate = db.date("f_datecash");
    QString docid = db.uuid();
    int partner = db.integer("f_partner");
    int doctype = db.integer("f_saletype") == 1 ? 20 : 5; //20 - retail, 5 - invoice
    db[":f_asdbid"] = fData["asdbid"].toInt();
    db[":f_table"] = "c_partners";
    db[":f_tableid"] = partner;
    db.exec("select * from as_convert where f_asdbid=:f_asdbid and f_table=:f_table and f_tableid=:f_tableid");
    int aspartner = 0;
    if (db.next()) {
        aspartner = db.integer("f_ascode");
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
    QString partnerLegalAddress = db.string("f_legaladdress");
    QString partnerTaxCode = db.string("f_taxcode");

    db[":f_header"] = orderUuid;
    db[":f_asdbid"] = fData["asdbid"].toInt();
    db[":f_pp"] = pricepolitic;
    db.exec("SELECT ds.f_goods, a.f_ascode, g.f_name, ds.f_price, ds.f_qty, "
            "if (:f_pp=1, gp.f_price1, gp.f_price2) as f_initprice, 0 as f_discount, "
            "g.f_service, a2.f_ascode as f_asstore "
            "FROM o_goods ds "
            "LEFT JOIN c_goods g ON g.f_id=ds.f_goods "
            "LEFT JOIN c_goods_prices gp ON gp.f_goods=g.f_id AND gp.f_currency=1 "
            "left join as_convert a on a.f_tableid=g.f_id and a.f_table='c_goods' and a.f_asdbid=:f_asdbid "
            "left join as_convert a2 on a2.f_tableid=ds.f_store and a2.f_table='c_storages' and a2.f_asdbid=:f_asdbid "
            "where ds.f_header=:f_header  ");
    double total = 0, vatamount = 0;
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
        tmp["f_initprice"] = doctype == 5 ? db.doubleValue("f_initprice") / 1.2 : db.doubleValue("f_initprice");
        tmp["f_price"] = doctype == 5 ? db.doubleValue("f_price") / 1.2 : db.doubleValue("f_price");
        tmp["f_discount"] = ((db.doubleValue("f_initprice") - db.doubleValue("f_price")) * 100) / db.doubleValue("f_initprice");
        tmp["f_service"] = db.integer("f_service");
        tmp["f_store"] = db.string("f_asstore");
        items.append(tmp);
        double ltotal = tmp["f_qty"].toDouble() * tmp["f_price"].toDouble();
        total += ltotal + (doctype == 5 ? (ltotal * 0.2) : 0);
        vatamount += doctype == 5 ? ltotal * 0.2 : ltotal * 0.1667 ;
    }
    if (!err.isEmpty()) {
        return false;
    }
    if (items.count() == 0) {
        err = tr("Empty invoice");
        return false;
    }

    dbas.startTransaction();
    dbas[":fISN"] = docid;
    dbas[":fDOCTYPE"] = fData["doctype"].toInt();
    dbas[":fDOCSTATE"] = 1;
    dbas[":fORDERNUM"] = "";
    dbas[":fDOCNUM"] = "";
    dbas[":fCUR"] = "AMD";
    dbas[":fSUMM"] = total;
    dbas[":fCOMMENT"] = partnersMap[aspartner]["fbusinessaddress"].isValid() ? partnersMap[aspartner]["fbusinessaddress"] : "";
    dbas[":fBODY"] = QString("\r\nPREPAYMENTACC:5231\r\nVATACC:5243\r\nSUMMVAT:%2\r\nBUYERACC:2211\r\n"
                "CUREXCHNG:1.0000\r\nCOURSECOUNT:1.0000\r\nBUYCHACCPOST:Գլխավոր հաշվապահ \r\nMAXROWID:%1\r\n"
                "BUYTAXCODE:%3\r\nBUYADDRESS:%4\r\nBUYBUSADDRESS:%5\r\nPOISN:00000000-0000-0000-0000-000000000000\r\nINVOICESTATES:0\r\n")
            .arg(items.count())
            .arg(vatamount)
                .arg(partnersMap[aspartner]["ftaxcode"].toString())
                .arg(partnersMap[aspartner]["faddress"].toString())
                .arg(partnersMap[aspartner]["fbusinessaddress"].toString());
    dbas[":fPARTNAME"] = partnersMap[aspartner]["fcaption"].isValid() ? partnersMap[aspartner]["fcaption"] : ""; // set to kamar
    dbas[":fUSERID"] = 0;
    dbas[":fPARTID"] = partnersMap[aspartner]["fpartid"];
    dbas[":fCRPARTID"] = -1;
    dbas[":fMTID"] = -1;
    dbas[":fINVN"] = "";
    dbas[":fENTRYSTATE"] = 0;
    dbas[":fEMPLIDRESPIN"] = -1;
    dbas[":fEMPLIDRESPOUT"] = -1;
    dbas[":fVATTYPE"] = doctype == 5 ? "1" : "5";
    dbas[":fSPEC"] = "                    00"; // <--- Tax receipt id
//        if (card > 0.001) {
//            dbas[":fBODY"] = dbas[":fBODY"].toString() +
//                    QString("BANKACQUIRINGACCOUNT:%1\r\nSUMMPLCARD:%2\r\n")
//                    .arg(bankacc)
//                    .arg(float_str(card, 2).replace(",", "").replace(" ", ""));
//        }
    if (dbas.exec("insert into DOCUMENTS ("
                  "fISN, fDOCTYPE, fDOCSTATE, fDATE, fORDERNUM, fDOCNUM, fCUR, fSUMM, fCOMMENT, fBODY, fPARTNAME, "
                  "fUSERID, fPARTID, fCRPARTID, fMTID, fEXPTYPE, fINVN, fENTRYSTATE, "
                  "fEMPLIDRESPIN, fEMPLIDRESPOUT, fVATTYPE, fSPEC, fCHANGEDATE,fEXTBODY,fETLSTATE) VALUES ("
                  ":fISN, :fDOCTYPE, :fDOCSTATE, '" + docDate.toString("yyyy-MM-dd") + "', "
                  ":fORDERNUM, :fDOCNUM, :fCUR, :fSUMM, :fCOMMENT, :fBODY, :fPARTNAME, "
                  ":fUSERID, :fPARTID, :fCRPARTID, :fMTID, '', :fINVN, :fENTRYSTATE, "
                  ":fEMPLIDRESPIN, :fEMPLIDRESPOUT, :fVATTYPE, :fSPEC, current_timestamp,'', '')") == false) {
        dbas.rollback();
        err = dbas.lastDbError();
        return false;
    }


    int rowid = 1;
    for (QList<QMap<QString, QVariant> >::const_iterator bi = items.constBegin(); bi != items.constEnd(); bi++) {

        dbas[":fISN"] = docid;
        dbas[":fROWNUM"] = rowid;
        dbas[":fITEMTYPE"] = (*bi)["f_service"].toInt() == 1 ? 2 : 1;
        dbas[":fITEMID"] = (*bi)["f_service"].toInt() == 1 ? servicesMap[(*bi)["f_ascode"].toString()]["fservid"] : goodsMap[(*bi)["f_ascode"].toString()]["fmtid"];
        dbas[":fITEMNAME"] = (*bi)["f_name"];
        dbas[":fUNITBRIEF"] = unitsMap[servicesMap[(*bi)["f_ascode"].toString()]["funit"].toString()];
        dbas[":fSTORAGE"] = (*bi)["f_store"];
        dbas[":fQUANTITY"] = (*bi)["f_qty"];
        dbas[":fINITPRICE"] = (*bi)["f_initprice"];
        dbas[":fDISCOUNT"] = (*bi)["f_discount"];
        dbas[":fPRICE"] = (*bi)["f_price"];
        dbas[":fSUMMA"] = (*bi)["f_qty"].toDouble() * (*bi)["f_price"].toDouble();
        dbas[":fSUMMA1"] = (*bi)["f_qty"].toDouble() * (*bi)["f_price"].toDouble();
        dbas[":fENVFEEPERCENT"] = 0;
        dbas[":fENVFEESUMMA"] = 0;
        dbas[":fVAT"] = 1;
        dbas[":fEXPMETHOD"] = 1;
        dbas[":fACCEXPENSE"] = (*bi)["f_service"].toInt() == 1 ? fData["lesexpenseacc"].toString() : fData["lemexpenseacc"].toString();
        dbas[":fACCINCOME"] = (*bi)["f_service"].toInt() == 1 ? fData["lesincomeacc"].toString() : fData["lemincomeacc"].toString();
        dbas[":fPARTYMETHOD"] = 0;
        dbas[":fROWID"] = rowid++;
        if (!dbas.exec("insert into MTINVOICELIST (fISN, fROWNUM, fITEMTYPE, fITEMID, fITEMNAME, "
                    "fUNITBRIEF, fSTORAGE, fQUANTITY, fINITPRICE, fDISCOUNT, fPRICE, fSUMMA, fSUMMA1, "
                    "fENVFEEPERCENT, fENVFEESUMMA, fVAT, fEXPMETHOD, fACCEXPENSE, fACCINCOME, fPARTYMETHOD, fROWID) "
                    "VALUES (:fISN, :fROWNUM, :fITEMTYPE, :fITEMID, :fITEMNAME, "
                    ":fUNITBRIEF, :fSTORAGE, :fQUANTITY, :fINITPRICE, :fDISCOUNT, :fPRICE, :fSUMMA, :fSUMMA1, "
                    ":fENVFEEPERCENT, :fENVFEESUMMA, :fVAT, :fEXPMETHOD, :fACCEXPENSE, :fACCINCOME, :fPARTYMETHOD, :fROWID)")) {
            err = dbas.lastDbError();
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

    q.exec("select fPARTID, fPARTCODE, fCAPTION, fFULLCAPTION, fADDRESS, fBUSINESSADDRESS, fTAXCODE from PARTNERS");
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
