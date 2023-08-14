#include "c5login.h"
#include "ui_c5login.h"
#include "c5connection.h"
#include "c5user.h"

C5Login::C5Login(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::C5Login)
{
    ui->setupUi(this);
    C5Database db(dbParams);
    db.exec("select f_login from s_user where f_id in (select f_user from s_db_access where f_permit=1)");
    while (db.nextRow()) {
        ui->leUsername->addItem(db.getString(0));
    }
    ui->leUsername->setCurrentIndex(-1);

    if (C5Config::fLastUsername.length() > 0) {
        ui->leUsername->setCurrentText(C5Config::fLastUsername);
        ui->lePassword->setFocus();
    }
    if (__autologin_store.contains("--autologin")) {
        for (const QString &s: __autologin_store) {
            if (s.contains("--user:")) {
                ui->leUsername->setCurrentText(s.mid(s.indexOf(":") + 1, s.length() - s.indexOf(":")));
            } else if (s.contains("--password:")) {
                ui->lePassword->setText(s.mid(s.indexOf(":") + 1, s.length() - s.indexOf(":")));
            }
        }
        on_btnOk_clicked();
    }

}

C5Login::~C5Login()
{
    delete ui;
}

int C5Login::exec()
{
    if (__autologin_store.count() > 0) {
        on_btnOk_clicked();
        return result();
    }
    return C5Dialog::exec();
}

void C5Login::on_btnCancel_clicked()
{
    reject();
}

void C5Login::on_btnOk_clicked()
{
    if (!__user) {
        __user = new C5User(0);
    }
    if (!__user->authByUsernamePass(ui->leUsername->currentText(), ui->lePassword->text())) {
        __autologin_store.clear();
        C5Message::error(tr("Login failed"));
        return;
    }
    if (!__user->isActive()) {
        __autologin_store.clear();
        C5Message::error(tr("User is inactive"));
        return;
    }

    C5Config::fLastUsername = ui->leUsername->currentText();
    C5Connection::writeParams();

    accept();
}

