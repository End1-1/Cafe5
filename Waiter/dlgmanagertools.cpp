#include "dlgmanagertools.h"
#include "ui_dlgmanagertools.h"
#include "dlgreports.h"
#include "c5user.h"
#include "dlgshiftrotation.h"
#include "dlgpassword.h"
#include "datadriver.h"
#include "dlgstafflist.h"

DlgManagerTools::DlgManagerTools(C5User *u) :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::DlgManagerTools)
{
    ui->setupUi(this);
    setWindowState(Qt::WindowFullScreen);
    fUser = u;
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
