#include "ce5user.h"
#include "ui_ce5user.h"
#include "c5cache.h"

CE5User::CE5User(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5User)
{
    ui->setupUi(this);
    ui->leState->setSelector(dbParams, ui->leStateName, cache_users_states);
    ui->leGroup->setSelector(dbParams, ui->leGroupName, cache_users_groups);
}

CE5User::~CE5User()
{
    delete ui;
}
