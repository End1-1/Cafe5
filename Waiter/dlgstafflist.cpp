#include "dlgstafflist.h"
#include "ui_dlgstafflist.h"
#include "datadriver.h"
#include "dlgpassword.h"
#include "c5config.h"
#include "c5message.h"
#include "c5database.h"
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
    dbuser->refresh();
    for (int id : dbuser->list()) {
        if (!txt.isEmpty()) {
            if (!dbuser->fullName(id).contains(txt, Qt::CaseInsensitive)) {
                continue;
            }
        }
        if (r > ui->tbl->rowCount() - 1) {
            ui->tbl->addEmptyRow();
        }
        ui->tbl->setString(r, c, dbuser->fullName(id));
        ui->tbl->item(r, c)->setData(Qt::UserRole, id);
        c++;
        if (c == ui->tbl->columnCount()) {
            c = 0;
            r++;
        }
    }
}

void DlgStaffList::on_tbl_cellClicked(int row, int column)
{
    int id = ui->tbl->item(row, column)->data(Qt::UserRole).toInt();
    if (id == 0) {
        return;
    }
    if (C5Message::question(tr("Are you sure to change password for ") + "\r\n" + ui->tbl->getString(row,
                            column)) != QDialog::Accepted) {
        return;
    }
    QString pass;
    if (!DlgPassword::getPasswordString(tr("Enter the new password for ") + "\r\n" + ui->tbl->getString(row, column),
                                        pass)) {
        return;
    }
    C5Database db;
    db[":f_altpassword"] = pass;
    db[":f_id"] = id;
    if (!db.exec("update s_user set f_altpassword=md5(:f_altpassword) where f_id=:f_id")) {
        C5Message::error(db.fLastError);
        return;
    }
    C5Message::info(tr("Password was changed for ") + "\r\n" + ui->tbl->getString(row, column));
}
