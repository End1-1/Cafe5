#include "ce5user.h"
#include "ui_ce5user.h"
#include "c5cache.h"
#include "ce5usergroup.h"

CE5User::CE5User(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5User)
{
    ui->setupUi(this);
    ui->leState->setSelector(dbParams, ui->leStateName, cache_users_states);
    ui->leGroup->setSelector(dbParams, ui->leGroupName, cache_users_groups);
    ui->leConfig->setSelector(dbParams, ui->leConfigName, cache_settings_names);
}

CE5User::~CE5User()
{
    delete ui;
}

bool CE5User::checkData(QString &err)
{
    bool result = CE5Editor::checkData(err);
    C5Database db(fDBParams);
    db[":f_id"] = ui->leCode->text();
    db[":f_login"] = ui->leLogin->text();
    db.exec("select * from s_user where f_login=:f_login and length(f_login)>0 and f_id<>:f_id");
    if (db.nextRow()) {
        err += tr("Duplicate login name") + "<br>";
        result = false;
    }
    return result;
}

void CE5User::on_btnNewGroup_clicked()
{
    CE5UserGroup *ep = new CE5UserGroup(fDBParams);
    C5Editor *e = C5Editor::createEditor(fDBParams, ep, 0);
    QList<QMap<QString, QVariant> > data;
    if(e->getResult(data)) {
        ui->leGroup->setValue(data.at(0)["f_id"].toString());
    }
    delete e;
}
