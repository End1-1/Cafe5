#include "c5passwords.h"
#include "ui_c5passwords.h"
#include "c5database.h"
#include "c5utils.h"
#include "c5message.h"

C5Passwords::C5Passwords() :
    C5Dialog(),
    ui(new Ui::C5Passwords)
{
    ui->setupUi(this);
    ui->leFrontPass->setEnabled(false);
    ui->leWaiterPass->setEnabled(false);
}

C5Passwords::~C5Passwords()
{
    delete ui;
}

void C5Passwords::setPasswords(int userId)
{
    C5Passwords *p = new C5Passwords();
    p->setUser(userId);
    p->exec();
    delete p;
}

void C5Passwords::on_btnOK_clicked()
{
    C5Database db;
    if (ui->chFrontPassword->isChecked()) {
        if (ui->leFrontPass->text().isEmpty()) {
            db[":f_password"] = "";
        } else {
            db[":f_password"] = password(ui->leFrontPass->text());
        }
        db.update("s_user", where_id(fUserId));
    }
    if (ui->chWaiterPassword->isChecked()) {
        db[":f_altpassword"] = password(ui->leWaiterPass->text());
        db.exec("update s_user set f_altpassword='' where f_altpassword=:f_altpassword");
        if (ui->leWaiterPass->text().isEmpty()) {
            db[":f_altpassword"] = "";
        } else {
            db[":f_altpassword"] = password(ui->leWaiterPass->text());
        }
        db.update("s_user", where_id(fUserId));
    }
    if (ui->chFrontPassword->isChecked() || ui->chWaiterPassword->isChecked()) {
        C5Message::info(tr("Saved"));
    }
    accept();
}

void C5Passwords::on_btnCancel_clicked()
{
    reject();
}

void C5Passwords::setUser(int userId)
{
    fUserId = userId;
    C5Database db;
    db[":f_id"] = userId;
    db.exec("select concat(f_last, ' ', f_first) from s_user where f_id=:f_id");
    if (db.nextRow()) {
        ui->lbStaff->setText(db.getString(0));
    }
}

void C5Passwords::on_chFrontPassword_clicked(bool checked)
{
    ui->leFrontPass->setEnabled(checked);
}

void C5Passwords::on_chWaiterPassword_clicked(bool checked)
{
    ui->leWaiterPass->setEnabled(checked);
}
