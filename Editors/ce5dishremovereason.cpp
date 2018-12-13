#include "ce5dishremovereason.h"
#include "ui_ce5dishremovereason.h"

CE5DishRemoveReason::CE5DishRemoveReason(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5DishRemoveReason)
{
    ui->setupUi(this);
}

CE5DishRemoveReason::~CE5DishRemoveReason()
{
    delete ui;
}
