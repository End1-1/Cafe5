#include "tax.h"
#include "configini.h"
#include "printtaxn.h"
#include "database.h"
#include "jsonhandler.h"
#include "shopmanager.h"
#include "databaseconnectionmanager.h"

Tax::Tax()
{

}

bool Tax::handle(const QByteArray &data, const QHash<QString, DataAddress> &dataMap)
{
    Database db;
    JsonHandler jh;
    if (!DatabaseConnectionManager::openDatabase(ShopManager::databaseName(), db, jh)) {
        return setInternalServerError(jh.toString());
    }
    QString sn, firm, address, fiscal, hvhh, rseq, devnum, time;
    PrintTaxN pt(ConfigIni::value("shop/taxip"),
                 ConfigIni::value("shop/taxport").toInt(),
                 ConfigIni::value("shop/taxpass"),
                 ConfigIni::value("shop/taxextpos"),
                 ConfigIni::value("shop/taxcashier"),
                 ConfigIni::value("shop/taxpin"),
                 this);
    double amountTotal = 0;
    db[":f_header"] = fOrder;
    db.exec("select gr.f_taxdept, gr.f_adgcode, og.f_qty, og.f_price, g.f_scancode, g.f_name "
            "from o_goods og "
            "left join c_goods g on g.f_id=og.f_goods "
            "left join c_groups gr on gr.f_id=g.f_group "
            "where og.f_header=:f_header");
    while(db.next()) {
        amountTotal += db.doubleValue("f_price") * db.doubleValue("f_qty");
        pt.addGoods(ConfigIni::value("shop/taxdep"), //dep
                    db.stringValue("f_adgcode"), //adg
                    db.stringValue("f_scancode"), //goods id
                    db.stringValue("f_name"), //name
                    db.doubleValue("f_price"), //price
                    db.doubleValue("f_qty"), //qty
                    0); //discount
    }
    QString jsonIn, jsonOut, err;
    int result = 0;
    result = pt.makeJsonAndPrint(0, 0, jsonIn, jsonOut, err);
    if (result == pt_err_ok) {
        PrintTaxN::parseResponse(jsonOut, firm, hvhh, fiscal, rseq, sn, address, devnum, time);
        db[":f_id"] = fOrder;
        db.exec("delete from o_tax where f_id=:f_id");
        db[":f_id"] = fOrder;
        db[":f_dept"] = ConfigIni::value("shop/taxdep");
        db[":f_firmname"] = firm;
        db[":f_address"] = address;
        db[":f_devnum"] = devnum;
        db[":f_serial"] = sn;
        db[":f_fiscal"] = fiscal;
        db[":f_receiptnumber"] = rseq;
        db[":f_hvhh"] = hvhh;
        db[":f_fiscalmode"] = tr("(F)");
        db[":f_time"] = time;
        db.insert("o_tax");
    } else {
        return setInternalServerError("<br>" + err + "<br>" + jsonIn);
    }
    jh["rseq"] = rseq;
    return setResponse(HTTP_OK, jh.toString());
}

bool Tax::validateData(const QByteArray &data, const QHash<QString, DataAddress> &dataMap)
{
    QString err;
    if (!dataMap.contains("auth")) {
        err += "Authentication method missing";
    } else {
        QStringList authMethods;
        authMethods.append("up");
        authMethods.append("key");
        for (const QString &am: authMethods) {
            if (getData(data, dataMap["auth"]) == am) {
                fAuthMethod = am;
                break;
            }
        }
        if (fAuthMethod.isEmpty()) {
            err += "Valid authentication method missing. ";
        } else {
            if (fAuthMethod == "up") {
                if (!dataMap.contains("user")) {
                    err += "Username reqired. ";
                } else {
                    fUser = getData(data, dataMap["user"]);
                }
                if (!dataMap.contains("pass")) {
                    err += "Password required";
                } else {
                    fPassword = getData(data, dataMap["pass"]);
                }
            } else if (fAuthMethod == "key") {
                if (!dataMap.contains("key")) {
                    err += "Key required. ";
                } else {
                    fKey = getData(data, dataMap["key"]);
                }
            }
        }
    }
    if (!dataMap.contains("order")) {
        err += "Order number required. ";
    } else {
        fOrder = getData(data, dataMap["order"]);
    }
    if (!err.isEmpty()) {
        return setDataValidationError(err);
    }
    return true;
}
