#include "dlgpassword.h"
#include "ui_dlgpassword.h"
#include "c5config.h"
#include "c5user.h"
#include "c5database.h"
#include <QCryptographicHash>
#include <QDoubleValidator>

DlgPassword::DlgPassword() :
    C5Dialog(QStringList()),
    ui(new Ui::DlgPassword)
{
    ui->setupUi(this);
    fMax = 0;
    fAutoFromDb = false;
}

DlgPassword::~DlgPassword()
{
    delete ui;
}

bool DlgPassword::getUser(const QString &title, C5User *user)
{
    DlgPassword *d = new DlgPassword();
    d->fUser = user;
    d->ui->label->setText(title);
    bool result = d->exec() == QDialog::Accepted;
    delete d;
    return result;
}

bool DlgPassword::getUserDB(const QString &title, C5User *user)
{
    DlgPassword *d = new DlgPassword();
    d->fUser = user;
    d->fAutoFromDb = true;
    d->ui->label->setText(title);
    bool result = d->exec() == QDialog::Accepted;
    delete d;
    return result;
}

bool DlgPassword::getQty(const QString &title, int &qty)
{
    DlgPassword *d = new DlgPassword();
    d->ui->label->setText(title);
    d->ui->lePassword->setEchoMode(QLineEdit::Normal);
    d->ui->lePassword->setMaxLength(2);
    bool result = d->exec() == QDialog::Accepted;
    qty = d->ui->lePassword->getInteger();
    delete d;
    return result;
}

bool DlgPassword::getAmount(const QString &title, double &amount, bool defaultAmount)
{
    DlgPassword *d = new DlgPassword();
    d->ui->label->setText(title);
    d->ui->lePassword->setEchoMode(QLineEdit::Normal);
    d->ui->lePassword->setValidator(new QDoubleValidator(0, amount, 2));
    if (defaultAmount) {
        d->ui->lePassword->setDouble(amount);
    }
    d->fMax = amount;
    bool result = d->exec() == QDialog::Accepted;
    amount = d->ui->lePassword->getDouble();
    delete d;
    return result;
}

void DlgPassword::handlePassword(const QJsonObject &obj)
{
    sender()->deleteLater();
    if (obj["reply"].toInt() == 0) {
        ui->lePassword->clear();
        C5Message::error(tr("Access denied"));
        ui->lePassword->setFocus();
        return;
    }
    fUser->fromJson(obj);
    __userid = fUser->fId;
    __username = fUser->fFull;
    accept();
}

void DlgPassword::on_pushButton_clicked()
{
    click("1");
}

void DlgPassword::click(const QString &text)
{
    ui->lePassword->setText(ui->lePassword->text() + text);
}

void DlgPassword::on_pushButton_2_clicked()
{
    click("2");
}

void DlgPassword::on_pushButton_3_clicked()
{
    click("3");
}

void DlgPassword::on_pushButton_4_clicked()
{
    click("4");
}

void DlgPassword::on_pushButton_5_clicked()
{
    click("5");
}

void DlgPassword::on_pushButton_6_clicked()
{
    click("6");
}

void DlgPassword::on_pushButton_7_clicked()
{
    click("7");
}

void DlgPassword::on_pushButton_8_clicked()
{
    click("8");
}

void DlgPassword::on_pushButton_9_clicked()
{
    click("9");
}

void DlgPassword::on_pushButton_10_clicked()
{
    click("0");
}

void DlgPassword::on_pushButton_11_clicked()
{
    reject();
}

void DlgPassword::on_pushButton_12_clicked()
{
    if (ui->lePassword->echoMode() == QLineEdit::Password) {
        if (fAutoFromDb) {
            C5Database db(C5Config::fDBHost, C5Config::fDBPath, C5Config::fDBUser, C5Config::fDBPassword);
            db[":f_altpassword"] = password(ui->lePassword->text());
            db.exec("select f_first, f_last, f_id from s_user where f_altpassword=:f_altpassword");
            if (db.nextRow()) {
                fUser->fFirst = db.getString(0);
                fUser->fLast = db.getString(1);
                fUser->fFull = fUser->fLast + " " + fUser->fFirst;
                fUser->fId = db.getInt(2);
                __userid = fUser->fId;
                __username = fUser->fFull;
                accept();
            } else {
                C5Message::error(tr("Access denied"));
            }
        } else {
            C5SocketHandler *s = createSocketHandler(SLOT(handlePassword(QJsonObject)));
            s->bind("cmd", sm_checkuser);
            s->bind("pass", password(ui->lePassword->text()));
            s->send();
        }
    } else {
        accept();
    }
}

void DlgPassword::on_lePassword_returnPressed()
{
    on_pushButton_12_clicked();
}

void DlgPassword::on_lePassword_textChanged(const QString &arg1)
{
    if (fMax > 0.01) {
        if (arg1.toDouble() > fMax) {
            ui->lePassword->setText(QString::number(fMax, 'f', 2).remove(QRegExp("\\.0+$")).remove(QRegExp("\\.$")));
        }
    }
}

void DlgPassword::on_btnClear_clicked()
{
    ui->lePassword->clear();
}
