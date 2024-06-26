#include "ce5storage.h"
#include "ui_ce5storage.h"

CE5Storage::CE5Storage(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5Storage)
{
    ui->setupUi(this);
    ui->cbState->setDBValues(dbParams, "select f_id, f_name from c_storages_state ");
}

CE5Storage::~CE5Storage()
{
    delete ui;
}

QString CE5Storage::title()
{
    return tr("Storage");
}

QString CE5Storage::table()
{
    return "c_storages";
}
