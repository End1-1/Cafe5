#include "dlgguestinfo.h"
#include "ui_dlgguestinfo.h"

DlgGuestInfo::DlgGuestInfo() :
    C5Dialog(__c5config.dbParams()),
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
    C5Database db(fDBParams);
    db[":f_contactname"] = ui->leContactName->text();
    db[":f_phone"] = ui->lePhoneNumber->text();
    if (fCode == 0) {
        fCode = db.insert("c_partners");
    } else {
        db.update("c_partners", "f_id", fCode);
    }
    C5Message::info(tr("Saved"));
    accept();
}
