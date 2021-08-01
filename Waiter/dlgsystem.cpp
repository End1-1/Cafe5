#include "dlgsystem.h"
#include "ui_dlgsystem.h"
#include "c5connection.h"

DlgSystem::DlgSystem(const QStringList &dbParams) :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::DlgSystem)
{
    ui->setupUi(this);
    showMaximized();
    hide();
}

DlgSystem::~DlgSystem()
{
    delete ui;
}

void DlgSystem::on_btnConnection_clicked()
{
    C5Dialog::go<C5Connection>(__c5config.dbParams());
}
