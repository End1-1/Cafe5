#include "c5login.h"
#include "ui_c5login.h"
#include "c5connection.h"

C5Login::C5Login(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::C5Login)
{
    ui->setupUi(this);
    if (C5Config::fLastUsername.length() > 0) {
        ui->leUsername->setText(C5Config::fLastUsername);
        ui->lePassword->setFocus();
    }
}

C5Login::~C5Login()
{
    delete ui;
}

void C5Login::on_btnCancel_clicked()
{
    reject();
}

void C5Login::on_btnOk_clicked()
{
    C5Database db(fDBParams);
    db[":f_login"] = ui->leUsername->text();
    db[":f_password"] = password(ui->lePassword->text());
    db.exec("select f_id, f_group, f_state, concat(f_last, ' ', f_first) from s_user where f_login=:f_login and f_password=:f_password");
    if (!db.nextRow()) {
        C5Message::error(tr("Login failed"));
        return;
    }
    if (db.getInt(2) == 0) {
        C5Message::error(tr("User is inactive"));
        return;
    }
    __userid = db.getInt(0);
    __usergroup = db.getInt(1);
    __username = db.getString(3);

    C5Config::fLastUsername = ui->leUsername->text();
    C5Connection::writeParams();

    accept();
}

