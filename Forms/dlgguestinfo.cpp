#include "dlgguestinfo.h"
#include "ui_dlgguestinfo.h"
#include "c5message.h"

DlgGuestInfo::DlgGuestInfo() :
    C5Dialog(),
    ui(new Ui::DlgGuestInfo)
{
    ui->setupUi(this);
    fCode = 0;
}

DlgGuestInfo::~DlgGuestInfo()
{
    delete ui;
}

void DlgGuestInfo::on_btnCancel_clicked()
{
    reject();
}

void DlgGuestInfo::on_btnSave_clicked()
{
    //TODO
    // C5Database db;
    // db[":f_contactname"] = ui->leContactName->text();
    // db[":f_phone"] = ui->lePhoneNumber->text();
    // if (fCode == 0) {
    //     fCode = db.insert("c_partners");
    // } else {
    //     db.update("c_partners", "f_id", fCode);
    // }
    // C5Message::info(tr("Saved"));
    // accept();
}
