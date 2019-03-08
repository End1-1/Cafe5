#include "ce5menuname.h"
#include "ui_ce5menuname.h"

CE5MenuName::CE5MenuName(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5MenuName)
{
    ui->setupUi(this);
}

CE5MenuName::~CE5MenuName()
{
    delete ui;
}

QString CE5MenuName::title()
{
    return tr("Menu name");
}

QString CE5MenuName::table()
{
    return "d_menu_names";
}

bool CE5MenuName::checkData(QString &err)
{
    bool result = CE5Editor::checkData(err);
    C5Database db(fDBParams);
    db[":f_name"] = ui->leName->text();
    db.exec("select * from d_menu_names where lower(f_name)=lower(:f_name)");
    if (db.nextRow()) {
        err += tr("Duplicate name") + "<br>";
        return false;
    }
    return result;
}
