#include "c5changepassword.h"
#include "ui_c5changepassword.h"
#include "c5message.h"
#include "c5database.h"
#include "c5config.h"
#include "c5user.h"

C5ChangePassword::C5ChangePassword() :
    C5OfficeDialog(),
    ui(new Ui::C5ChangePassword)
{
    ui->setupUi(this);
}

C5ChangePassword::~C5ChangePassword()
{
    delete ui;
}

bool C5ChangePassword::changePassword(QString &password)
{
    C5ChangePassword *d = new C5ChangePassword();
    bool result = d->exec() == QDialog::Accepted;
    password = d->ui->leNewPassword->text();
    delete d;
    return result;
}

void C5ChangePassword::on_btnCancel_clicked()
{
    reject();
}

void C5ChangePassword::on_btnOK_clicked()
{
    if(ui->leNewPassword->text() != ui->leConfirmPassword->text()) {
        C5Message::error(tr("Password does not match"));
        return;
    }

    if(ui->leNewPassword->isEmpty()) {
        C5Message::error(tr("Password cannot be empty"));
        return;
    }

    C5Database db;
    db[":f_id"] = mUser->id();
    db[":f_password"] = ui->leOldPassword->text();
    db.exec("select * from s_user where f_id=:f_id and f_password=md5(:f_password)");

    if(!db.nextRow()) {
        C5Message::error(tr("Old password doesnt match"));
        return;
    }

    db[":f_id"] = mUser->id();
    db[":f_password"] = ui->leNewPassword->text();
    db.exec("update s_user set f_password=md5(:f_password) where f_id=:f_id");
    accept();
}
