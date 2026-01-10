#include "dlgpassword.h"
#include "ui_dlgpassword.h"
#include "c5config.h"
#include "c5user.h"
#include "c5message.h"
#include <QCryptographicHash>
#include <QDoubleValidator>

DlgPassword::DlgPassword(C5User *u) :
    C5Dialog(u),
    ui(new Ui::DlgPassword)
{
    ui->setupUi(this);
    fMax = 0;
    ui->btnRemoveFromStopList->setVisible(false);
}

DlgPassword::~DlgPassword()
{
    delete ui;
}

bool DlgPassword::getQty(const QString &title, int& qty)
{
    C5User u;
    DlgPassword *d = new DlgPassword(&u);
    d->ui->label->setText(title);
    d->ui->lePassword->setEchoMode(QLineEdit::Normal);
    d->ui->lePassword->setMaxLength(2);
    bool result = d->exec() == QDialog::Accepted;
    qty = d->ui->lePassword->getInteger();
    delete d;
    return result;
}

bool DlgPassword::stopList(int& qty)
{
    DlgPassword *d = new DlgPassword(nullptr);
    d->ui->label->setVisible(false);
    d->ui->btnRemoveFromStopList->setVisible(true);
    d->ui->lePassword->setEchoMode(QLineEdit::Normal);
    d->ui->lePassword->setMaxLength(3);
    bool result = d->exec() == QDialog::Accepted;
    qty = d->ui->lePassword->getInteger();
    delete d;
    return result;
}

bool DlgPassword::getAmount(const QString &title, double& amount, bool defaultAmount)
{
    C5User u;
    DlgPassword *d = new DlgPassword(&u);
    d->ui->label->setText(title);
    d->ui->lePassword->setEchoMode(QLineEdit::Normal);
    d->ui->lePassword->setValidator(new QDoubleValidator(0, amount, 2));

    if(defaultAmount) {
        d->ui->lePassword->setDouble(amount);
    }

    d->fMax = amount;
    bool result = d->exec() == QDialog::Accepted;
    amount = d->ui->lePassword->getDouble();
    delete d;
    return result;
}

bool DlgPassword::getString(const QString &title, QString &str)
{
    C5User u;
    DlgPassword *d = new DlgPassword(&u);
    d->ui->label->setText(title);
    d->ui->lePassword->setEchoMode(QLineEdit::Normal);
    d->ui->lePassword->setMaxLength(20);
    bool result = d->exec() == QDialog::Accepted;
    str = d->ui->lePassword->text();
    delete d;
    return result;
}

bool DlgPassword::getPassword(const QString &title, QString &str)
{
    C5User u;
    DlgPassword *d = new DlgPassword(&u);
    d->ui->label->setText(title);
    d->ui->lePassword->setEchoMode(QLineEdit::Password);
    d->ui->lePassword->setMaxLength(20);
    d->setProperty("pass", true);
    bool result = d->exec() == QDialog::Accepted;
    str = d->ui->lePassword->text().replace(";", "").replace("?", "");
    delete d;
    return result;
}

bool DlgPassword::getPasswordString(const QString &title, QString &pass)
{
    C5User u;
    DlgPassword *d = new DlgPassword(&u);
    d->setProperty("pass", true);
    d->ui->label->setText(title);
    d->ui->lePassword->setEchoMode(QLineEdit::Password);
    d->ui->lePassword->setMaxLength(20);
    bool result = d->exec() == QDialog::Accepted;
    pass = d->ui->lePassword->text().replace(";", "").replace("?", "");
    delete d;
    return result;
}

void DlgPassword::showEvent(QShowEvent *e)
{
    C5Dialog::showEvent(e);
    adjustSize();
}

void DlgPassword::on_pushButton_clicked()
{
    click("1");
}

void DlgPassword::click(const QString &text)
{
    if(text == ".") {
        if(ui->lePassword->text().contains(".")) {
            return;
        }
    }

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
    QString pwd = ui->lePassword->text().replace(";", "").replace("?", "");

    if(property("pass").toBool()) {
        accept();
        return;
    }

    if(ui->lePassword->echoMode() == QLineEdit::Password) {
        fUser->authorize(pwd, fHttp, [this](const QJsonObject & jdoc) {
            Q_UNUSED(jdoc);
            accept();
        }, []() {
        });
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
    if(fMax > 0.01) {
        if(arg1.toDouble() > fMax) {
            ui->lePassword->setText(QString::number(fMax, 'f',
                                                    2).remove(QRegularExpression("\\.0+$")).remove(QRegularExpression("\\.$")));
        }
    }
}

void DlgPassword::on_btnClear_clicked()
{
    ui->lePassword->clear();
}

void DlgPassword::on_pushButton_13_clicked()
{
    click(".");
}

void DlgPassword::on_btnRemoveFromStopList_clicked()
{
    ui->lePassword->setText("-999");
    accept();
}
