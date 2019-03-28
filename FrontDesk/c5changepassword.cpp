#include "c5changepassword.h"
#include "ui_c5changepassword.h"

C5ChangePassword::C5ChangePassword(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::C5ChangePassword)
{
    ui->setupUi(this);
}

C5ChangePassword::~C5ChangePassword()
{
    delete ui;
}

bool C5ChangePassword::changePassword(const QStringList &dbParams, QString &password)
{
    C5ChangePassword *d = new C5ChangePassword(dbParams);
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
    if (ui->leNewPassword->text() != ui->leConfirmPassword->text()) {
        C5Message::error(tr("Password does not match"));
        return;
    }
    if (ui->leNewPassword->isEmpty()) {
        C5Message::error(tr("Password cannot be empty"));
        return;
    }
    C5Database db(fDBParams);
    db[":f_id"] = __userid;
    db[":f_password"] = ui->leOldPassword->text();
    db.exec("select * from s_user where f_id=:f_id and f_password=md5(:f_password)");
    if (!db.nextRow()) {
        C5Message::error(tr("Old password doesnt match"));
        return;
    }
    accept();
}
