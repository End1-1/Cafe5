#include "dlgsystem.h"
#include "ui_dlgsystem.h"
#include "c5connection.h"
#include <QProcess>
#include <QSettings>

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
    QSettings ss(_ORGANIZATION_, _APPLICATION_+ QString("\\") + _MODULE_);
    QJsonObject js = QJsonDocument::fromJson(ss.value("server", "{}").toByteArray()).object();
    C5Connection c(js);
    if (c.exec() == QDialog::Accepted) {
        js = c.fParams;
        ss.setValue("server", QJsonDocument(js).toJson(QJsonDocument::Compact));
    }

    C5Config::fDBHost = js["host"].toString();
    C5Config::fDBPath = js["database"].toString();
    C5Config::fDBUser = js["username"].toString();
    C5Config::fDBPassword = js["password"].toString();
    C5Config::fSettingsName = js["settings"].toString();
    C5Config::fFullScreen = true;
    C5Config::initParamsFromDb();
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
