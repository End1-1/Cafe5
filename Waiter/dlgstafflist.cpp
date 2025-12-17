#include "dlgstafflist.h"
#include "ui_dlgstafflist.h"
#include "dlgpassword.h"
#include "c5message.h"
#include <QScreen>

DlgStaffList::DlgStaffList() :
    C5Dialog(),
    ui(new Ui::DlgStaffList)
{
    ui->setupUi(this);
    setWindowState(Qt::WindowFullScreen);
    connect(ui->wkbd, &RKeyboard::textChanged, this, &DlgStaffList::searchStaff);
    connect(ui->wkbd, &RKeyboard::reject, this, &DlgStaffList::reject);
    QRect scr = qApp->screens().at(0)->geometry();
    setProperty("cols", (scr.width() - 20) / ui->tbl->horizontalHeader()->defaultSectionSize());
    searchStaff("");
}

DlgStaffList::~DlgStaffList()
{
    delete ui;
}

void DlgStaffList::searchStaff(const QString &txt)
{
    ui->tbl->clearContents();
    ui->tbl->setColumnCount(property("cols").toInt());
    ui->tbl->setRowCount(0);
    int c = 0, r = 0;
}

void DlgStaffList::on_tbl_cellClicked(int row, int column)
{
    int id = ui->tbl->item(row, column)->data(Qt::UserRole).toInt();

    if(id == 0) {
        return;
    }

    if(C5Message::question(tr("Are you sure to change password for ") + "\r\n" + ui->tbl->getString(row,
                           column)) != QDialog::Accepted) {
        return;
    }

    QString pass;

    if(!DlgPassword::getPasswordString(tr("Enter the new password for ") + "\r\n" + ui->tbl->getString(row, column),
                                       pass)) {
        return;
    }

    //TODO
    // C5Database db;
    // db[":f_altpassword"] = pass;
    // db[":f_id"] = id;
    // if(!db.exec("update s_user set f_altpassword=md5(:f_altpassword) where f_id=:f_id")) {
    //     C5Message::error(db.fLastError);
    //     return;
    // }
    C5Message::info(tr("Password was changed for ") + "\r\n" + ui->tbl->getString(row, column));
}
