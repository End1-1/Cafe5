#include "config.h"
#include "c5database.h"
#include <QJsonDocument>

Config::Config(const QStringList &dbParams, int id) :
    fDbParams(dbParams),
    fId(id)
{
    C5Database db(dbParams);
    db[":f_id"] = fId;
    db.exec("select * from sys_configs where f_id=:f_id");
    if (db.nextRow()) {
        jo = QJsonDocument::fromJson(db.getString("f_data").toUtf8()).object();
    }
}
