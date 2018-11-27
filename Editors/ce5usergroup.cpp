#include "ce5usergroup.h"
#include "ui_ce5usergroup.h"

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
