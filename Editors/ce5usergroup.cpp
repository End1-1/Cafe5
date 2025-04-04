#include "ce5usergroup.h"
#include "ui_ce5usergroup.h"
#include "c5message.h"

CE5UserGroup::CE5UserGroup(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5UserGroup)
{
    ui->setupUi(this);
}

CE5UserGroup::~CE5UserGroup()
{
    delete ui;
}

void CE5UserGroup::clear()
{
    CE5Editor::clear();
    ui->leFixed->clear();
    ui->leMin->clear();
    ui->leMax->clear();
    ui->leValDep->clear();
    ui->leValTotal->clear();
    ui->lwOwnTotal->clear();
}

void CE5UserGroup::setId(int id)
{
    CE5Editor::setId(id);
    C5Database db(fDBParams);
    db[":f_position"] = id;
    db.exec("select * from s_salary_rules where f_position=:f_position");
    if (db.nextRow()) {
        ui->leFixed->setDouble(db.getDouble("f_fixed"));
        ui->leMin->setDouble(db.getDouble("f_min"));
        ui->leMax->setDouble(db.getDouble("f_max"));
        ui->leValDep->setInteger(db.getInt("f_dep"));
        ui->leValTotal->setDouble(db.getDouble("f_total"));
        ui->lwOwnTotal->setDouble(db.getDouble("f_owntotal"));
    }
}

bool CE5UserGroup::save(QString &err, QList<QMap<QString, QVariant> > &data)
{
    if (!CE5Editor::save(err, data)) {
        C5Message::error(err);
        return false;
    }
    C5Database db(fDBParams);
    db[":f_position"] = ui->leCode->getInteger();
    db.exec("delete from s_salary_rules where f_position=:f_position");
    db[":f_position"] = ui->leCode->getInteger();
    db[":f_fixed"] = ui->leFixed->getDouble();
    db[":f_min"] = ui->leMin->getDouble();
    db[":f_max"] = ui->leMax->getDouble();
    db[":f_dep"] = ui->leValDep->getInteger();
    db[":f_owntotal"] = ui->lwOwnTotal->getDouble();
    db[":f_total"] = ui->leValTotal->getDouble();
    db.insert("s_salary_rules", false);
    return true;
}
