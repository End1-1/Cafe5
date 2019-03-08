#include "ce5table.h"
#include "ui_ce5table.h"
#include "c5cache.h"

CE5Table::CE5Table(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5Table)
{
    ui->setupUi(this);
    ui->leHallCode->setSelector(dbParams, ui->leHallName, cache_halls);
}

CE5Table::~CE5Table()
{
    delete ui;
}
