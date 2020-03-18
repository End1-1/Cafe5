#include "sockethandlergoods.h"
#include "c5database.h"
#include "config.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

SocketHandlerGoods::SocketHandlerGoods(QByteArray &data) :
    SocketHandler(data)
{

}

void SocketHandlerGoods::processData()
{
    fData.clear();
    C5Database db(DBHOST, DBFILE, DBUSER, DBPASSWORD);
    QJsonArray jagroups;
    db.exec("select f_id, f_name from c_groups");
    while (db.nextRow()) {
        QJsonObject j;
        j["f_id"] = db.getInt(0);
        j["f_name"] = db.getString(1);
        jagroups.append(j);
    }
    QJsonArray jagoods;
    db.exec("select f_id, f_group, f_name, f_saleprice, f_saleprice2 from c_goods where f_enabled=1 order by f_group, f_name");
    while (db.nextRow()) {
        QJsonObject j;
        j["f_id"] = db.getInt("f_id");
        j["f_group"] = db.getInt("f_group");
        j["f_name"] = db.getString("f_name");
        j["f_saleprice"] = db.getDouble("f_saleprice");
        j["f_saleprice2"] = db.getDouble("f_saleprice2");
        jagoods.append(j);
    }
    QJsonArray jpartners;
    db.exec("select f_taxcode, f_taxname, f_contact, f_info, f_phone, f_address from c_partners");
    while (db.nextRow()) {
        QJsonObject j;
        for (int i = 0; i < db.columnCount(); i++) {
            j[db.columnName(i)] = db.getString(i);
        }
        jpartners.append(j);
    }
    QJsonObject jo;
    jo["groups"] = jagroups;
    jo["goods"] = jagoods;
    jo["partners"] = jpartners;
    QJsonDocument jdoc(jo);
    fData = jdoc.toJson(QJsonDocument::Compact);
}
