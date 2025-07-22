#include "ce5halls.h"
#include "ui_ce5halls.h"
#include "c5cache.h"

CE5Halls::CE5Halls(QWidget *parent) :
    CE5Editor(parent),
    ui(new Ui::CE5Halls)
{
    ui->setupUi(this);
    ui->leSettings->setSelector(ui->leSettingsName, cache_settings_names);
}

CE5Halls::~CE5Halls()
{
    delete ui;
}
