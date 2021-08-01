#include "dlgmanagertools.h"
#include "ui_dlgmanagertools.h"
#include "dlgreports.h"
#include "c5user.h"
#include "dlgshiftrotation.h"
#include "dlgpassword.h"
#include "datadriver.h"

DlgManagerTools::DlgManagerTools(C5User *u) :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::DlgManagerTools)
{
    ui->setupUi(this);
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
