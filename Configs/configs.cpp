#include "configs.h"
#include <QJsonDocument>

Configs::Configs(int id) :
    fId(id)
{
    // TODO
    // C5Database db;
    // db[":f_id"] = fId;
    // db.exec("select * from sys_configs where f_id=:f_id");
    // if(db.nextRow()) {
    //     jo = QJsonDocument::fromJson(db.getString("f_data").toUtf8()).object();
    // }
}
