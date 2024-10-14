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
    QJsonObject jh = fData["header"].toObject();
    QJsonArray jb = fData["body"].toArray();
    QDate docDate = QDate::fromString(jh["f_datecash"].toString(), "yyyy-MM-dd");
    QString docid = Database::uuid();
    int partner = jh["f_partner"].toInt();
    //int doctype = db.integer("f_saletype") == 1 ? 20 : 5; //20 - retail, 5 - invoice
    int doctype = jh["f_saletype"].toInt();
    int aspartner = jh["f_aspartnerid"].toInt();
    int pricepolitic = jh["f_price_politic"].toInt();
    QString partnerAddress = jh["f_address"].toString();
    QString partnerLegalAddress = jh["f_legaladdress"].toString();
    QString partnerTaxCode = jh["f_taxcode"].toString();
    double total = 0, vatamount = 0;
    QList<QMap<QString, QVariant> > items;
    for (int i = 0; i < jb.size(); i++) {
        QJsonObject jt = jb.at(i).toObject();
        QMap<QString, QVariant> tmp;
        tmp["f_ascode"] = jt["f_ascode"].toString();
        tmp["f_goods"] = jt["f_goods"].toInt();
        tmp["f_name"] = jt["f_name"].toString();
        tmp["f_qty"] = jt["f_qty"].toDouble();
        tmp["f_initprice"] =  jt["f_initprice"].toDouble() / fData["pricewithoutvat"].toDouble() ;
        tmp["f_price"] =  jt["f_price"].toDouble() / fData["pricewithoutvat"].toDouble();
        tmp["f_discount"] = jt["f_discount"].toDouble() * 100;
        tmp["f_service"] = jt["f_service"].toInt();
        tmp["f_store"] = jt["f_asstore"].toString();
        items.append(tmp);
        double ltotal = tmp["f_qty"].toDouble() * tmp["f_price"].toDouble();
        total += ltotal + (ltotal *fData["withvat"].toDouble());
        //vatamount += doctype == 5 ? ltotal * 0.2 : ltotal * 0.1667 ;
        vatamount += ltotal *fData["vatpercent"].toDouble();
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
    dbas[":fCOMMENT"] = partnerAddress;
    dbas[":fBODY"] = QString("\r\nPREPAYMENTACC:5231\r\nVATACC:5243\r\nSUMMVAT:%2\r\nBUYERACC:2211\r\n"
                             "CUREXCHNG:1.0000\r\nCOURSECOUNT:1.0000\r\nBUYCHACCPOST:Գլխավոր հաշվապահ \r\nMAXROWID:%1\r\n"
                             "BUYTAXCODE:%3\r\nBUYADDRESS:%4\r\nBUYBUSADDRESS:%5\r\nPOISN:00000000-0000-0000-0000-000000000000\r\nINVOICESTATES:0\r\n")
                     .arg(items.count())
                     .arg(vatamount)
                     .arg(partnersMap[aspartner]["ftaxcode"].toString())
                     .arg(partnersMap[aspartner]["faddress"].toString())
                     .arg(partnerAddress);
    dbas[":fPARTNAME"] = partnersMap[aspartner]["fcaption"].isValid() ? partnersMap[aspartner]["fcaption"] :
                         ""; // set to kamar
    dbas[":fUSERID"] = 0;
    dbas[":fPARTID"] = partnersMap[aspartner]["fpartid"];
    dbas[":fCRPARTID"] = -1;
    dbas[":fMTID"] = -1;
    dbas[":fINVN"] = "";
    dbas[":fENTRYSTATE"] = 0;
    dbas[":fEMPLIDRESPIN"] = -1;
    dbas[":fEMPLIDRESPOUT"] = -1;
    dbas[":fVATTYPE"] = fData["vattype"].toString();// doctype == 5 ? "1" : "3";
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
        dbas[":fITEMTYPE"] = ( *bi)["f_service"].toInt() == 1 ? 2 : 1;
        dbas[":fITEMID"] = ( *bi)["f_service"].toInt() == 1 ? servicesMap[( *bi)["f_ascode"].toString()]["fservid"] :
                           goodsMap[( *bi)["f_ascode"].toString()]["fmtid"];
        dbas[":fITEMNAME"] = ( *bi)["f_name"];
        dbas[":fUNITBRIEF"] = unitsMap[servicesMap[( *bi)["f_ascode"].toString()]["funit"].toString()];
        dbas[":fSTORAGE"] = ( *bi)["f_store"];
        dbas[":fQUANTITY"] = ( *bi)["f_qty"];
        dbas[":fINITPRICE"] = ( *bi)["f_initprice"];
        dbas[":fDISCOUNT"] = ( *bi)["f_discount"];
        dbas[":fPRICE"] = ( *bi)["f_price"];
        dbas[":fSUMMA"] = ( *bi)["f_qty"].toDouble() * ( *bi)["f_price"].toDouble();
        dbas[":fSUMMA1"] = ( *bi)["f_qty"].toDouble() * ( *bi)["f_price"].toDouble();
        dbas[":fENVFEEPERCENT"] = 0;
        dbas[":fENVFEESUMMA"] = 0;
        dbas[":fVAT"] = 1;
        dbas[":fEXPMETHOD"] = 1;
        dbas[":fACCEXPENSE"] = ( *bi)["f_service"].toInt() == 1 ? fData["lesexpenseacc"].toString() :
                               fData["lemexpenseacc"].toString();
        dbas[":fACCINCOME"] = ( *bi)["f_service"].toInt() == 1 ? fData["lesincomeacc"].toString() :
                              fData["lemincomeacc"].toString();
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
