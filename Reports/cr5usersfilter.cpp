#include "cr5usersfilter.h"
#include "ui_cr5usersfilter.h"
#include "c5cache.h"

CR5UsersFilter::CR5UsersFilter() :
    C5FilterWidget(),
    ui(new Ui::CR5UsersFilter)
{
    ui->setupUi(this);
    ui->leGroup->setSelector(ui->leGroupName, cache_users_groups);
    ui->leState->setSelector(ui->leStateName, cache_users_states);
}

CR5UsersFilter::~CR5UsersFilter()
{
    delete ui;
}

QString CR5UsersFilter::condition()
{
    QString cond = " where u.f_id>0 ";
    if (!ui->leGroup->isEmpty()) {
        cond += QString(" and u.f_group in (%1) ").arg(ui->leGroup->text());
    }
    if (!ui->leState->isEmpty()) {
        cond += QString(" and u.f_state in (%1) ").arg(ui->leState->text());
    }
    return cond;
}

QString CR5UsersFilter::filterText()
{
    QString s;
    inFilterText(s, ui->leStateName);
    inFilterText(s, ui->leGroupName);
    return s;
}
