#include "dlgsystem.h"
#include "ui_dlgsystem.h"
#include "c5connection.h"
#include <QProcess>

DlgSystem::DlgSystem(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::DlgSystem)
{
    ui->setupUi(this);
    setWindowState(Qt::WindowFullScreen);
}

DlgSystem::~DlgSystem()
{
    delete ui;
}

void DlgSystem::setupNoPassword()
{
    ui->btnConnection->setEnabled(false);
}

void DlgSystem::on_btnConnection_clicked()
{
    C5Dialog::go<C5Connection>(__c5config.dbParams());
}

void DlgSystem::on_btnShutdown_clicked()
{
    if (C5Message::question(tr("Confirm to shutdown the system")) == QDialog::Accepted) {
        QProcess p;
        p.startDetached("shutdown -f -t 1 -s");
    }
}

void DlgSystem::on_btnRestart_clicked()
{
    if (C5Message::question(tr("Confirm to reboot the system")) == QDialog::Accepted) {
        QProcess p;
        p.startDetached("shutdown -f -t 1 -r");
    }
}

void DlgSystem::on_btnExit_clicked()
{
    accept();
}

void DlgSystem::on_btnExitToWindows_clicked()
{
    if (C5Message::question(tr("Exit to windows")) == QDialog::Accepted) {
        qApp->exit();
    }
}
