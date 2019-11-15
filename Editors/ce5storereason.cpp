#include "ce5storereason.h"
#include "ui_ce5storereason.h"

CE5StoreReason::CE5StoreReason(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5StoreReason)
{
    ui->setupUi(this);
}

CE5StoreReason::~CE5StoreReason()
{
    delete ui;
}


QString CE5StoreReason::title()
{
    return tr("Movement reason");
}

QString CE5StoreReason::table()
{
    return "a_reason";
}


bool CE5StoreReason::checkData(QString &err)
{
    if (ui->leCode->getInteger() < 10) {
        err += tr("This item is not editable") + "<br>";
        return false;
    }
}
