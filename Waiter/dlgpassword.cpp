#include "dlgpassword.h"
#include "ui_dlgpassword.h"
#include "c5config.h"
#include "c5user.h"
#include <QCryptographicHash>

DlgPassword::DlgPassword(QWidget *parent) :
    C5Dialog(parent),
    ui(new Ui::DlgPassword)
{
    ui->setupUi(this);
}

DlgPassword::~DlgPassword()
{
    delete ui;
}

bool DlgPassword::getUser(const QString &title, C5User *user)
{
    DlgPassword *d = new DlgPassword(C5Config::fParentWidget);
    d->fUser = user;
    d->ui->label->setText(title);
    bool result = d->exec() == QDialog::Accepted;
    delete d;
    return result;
}

bool DlgPassword::getQty(const QString &title, int &qty)
{
    DlgPassword *d = new DlgPassword(C5Config::fParentWidget);
    d->ui->label->setText(title);
    d->ui->lePassword->setEchoMode(QLineEdit::Normal);
    d->ui->lePassword->setMaxLength(2);
    bool result = d->exec() == QDialog::Accepted;
    qty = d->ui->lePassword->text().toInt();
    delete d;
    return result;
}

void DlgPassword::handlePassword(const QJsonObject &obj)
{
    sender()->deleteLater();
    if (obj["reply"].toInt() == 0) {
        ui->lePassword->clear();
        C5Message::error(tr("Access denied"));
        return;
    }
    fUser->fromJson(obj);
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
        C5SocketHandler *s = createSocketHandler(SLOT(handlePassword(QJsonObject)));
        s->bind("cmd", sm_checkuser);
        s->bind("pass", QCryptographicHash::hash(ui->lePassword->text().toLatin1(), QCryptographicHash::Md5).toHex());
        s->send();
    } else {
        accept();
    }
}

void DlgPassword::on_lePassword_returnPressed()
{
    on_pushButton_12_clicked();
}
