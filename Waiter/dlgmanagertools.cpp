#include "dlgmanagertools.h"
#include "ui_dlgmanagertools.h"
#include "dlgreports.h"
#include "c5user.h"
#include "dlgshiftrotation.h"
#include "dlgpassword.h"
#include "dlgstafflist.h"
#include "c5message.h"
#include "c5user.h"
#include "c5permissions.h"
#include "c5config.h"
#include "c5database.h"
#include <QSettings>
#include <QProcess>

DlgManagerTools::DlgManagerTools(C5User *u) :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::DlgManagerTools)
{
    ui->setupUi(this);
    setWindowState(Qt::WindowFullScreen);
    fUser = u;
    QSettings _ls(qApp->applicationDirPath() + "/ls.inf", QSettings::IniFormat);
    if (_ls.value("mt/session", 0).toInt() > 0) {
        ui->btnChangeSession->setEnabled(_ls.value("mt/session", -1).toInt() == __user->id());
    }
    if (_ls.value("mt/mypass", 0).toInt() > 0) {
        ui->btnChangeMyPassword->setEnabled(_ls.value("mt/mypass", -1).toInt() == __user->id());
    }
    if (_ls.value("mt/staffpass", 0).toInt() > 0) {
        ui->btnChangeStaffPassword->setEnabled(_ls.value("mt/staffpass", -1).toInt() == __user->id());
    }
}

DlgManagerTools::~DlgManagerTools()
{
    delete ui;
}

void DlgManagerTools::on_pushButton_clicked()
{
    DlgReports::openReports(fUser);
}

void DlgManagerTools::on_btnChangeSession_clicked()
{
    C5User *tmp = fUser;
    if (!tmp->check(cp_t5_shift_rotation)) {
        if (!DlgPassword::getUserAndCheck(tr("Change session"), tmp, cp_t5_shift_rotation)) {
            return;
        }
    }
    DlgShiftRotation d(tmp);
    d.exec();
    if (tmp != fUser) {
        delete tmp;
    }
}

void DlgManagerTools::on_btnChangeMyPassword_clicked()
{
    if (C5Message::question(tr("Are you sure change your password?")) != QDialog::Accepted) {
        return;
    }
    QString pass;
    if (!DlgPassword::getPasswordString(tr("Enter the new password"), pass)) {
        return;
    }
    C5Database db(__c5config.dbParams());
    db[":f_altpassword"] = pass;
    db[":f_id"] = fUser->id();
    if (!db.exec("update s_user set f_altpassword=md5(:f_altpassword) where f_id=:f_id")) {
        C5Message::error(db.fLastError);
        return;
    }
    C5Message::info(tr("Your password was changed"));
}

void DlgManagerTools::on_btnChangeStaffPassword_clicked()
{
    DlgStaffList d;
    d.exec();
}

void DlgManagerTools::on_btnExit_clicked()
{
    accept();
}

void DlgManagerTools::on_btnExitToWindows_clicked()
{
    if (C5Message::question(tr("Exit to windows")) == QDialog::Accepted) {
        qApp->exit();
    }
}

void DlgManagerTools::on_btnShutdown_clicked()
{
    if (C5Message::question(tr("Confirm to shutdown the system")) == QDialog::Accepted) {
        QProcess p;
        p.startDetached("shutdown -f -t 1 -s");
    }
}

void DlgManagerTools::on_btnRestart_clicked()
{
    if (C5Message::question(tr("Confirm to reboot the system")) == QDialog::Accepted) {
        QProcess p;
        p.startDetached("shutdown -f -t 1 -r");
    }
}
