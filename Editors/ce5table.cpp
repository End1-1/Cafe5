#include "ce5table.h"
#include "ui_ce5table.h"
#include "c5cache.h"

CE5Table::CE5Table(QWidget *parent) :
    CE5Editor(parent),
    ui(new Ui::CE5Table)
{
    ui->setupUi(this);
    ui->leHallCode->setSelector(ui->leHallName, cache_halls);
    ui->leConfig->setSelector(ui->leConfigName, cache_settings_names);
}

CE5Table::~CE5Table()
{
    delete ui;
}
