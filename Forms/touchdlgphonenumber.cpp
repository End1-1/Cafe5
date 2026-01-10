#include "touchdlgphonenumber.h"
#include "ui_touchdlgphonenumber.h"
#include "c5config.h"

TouchDlgPhoneNumber::TouchDlgPhoneNumber(C5User *user) :
    C5Dialog(user),
    ui(new Ui::TouchDlgPhoneNumber)
{
    ui->setupUi(this);
}

TouchDlgPhoneNumber::~TouchDlgPhoneNumber()
{
    delete ui;
}

bool TouchDlgPhoneNumber::getPhoneNumber(QString &number, C5User *user)
{
    TouchDlgPhoneNumber d(user);

    if(d.exec() == QDialog::Accepted) {
        number = d.ui->lePhone->text();
        return true;
    }

    return false;
}

void TouchDlgPhoneNumber::on_btnOk_clicked()
{
    if(ui->lePhone->text().isEmpty() == false) {
        accept();
    }
}

void TouchDlgPhoneNumber::buttonClicked(QPushButton *b)
{
    QString p = ui->lePhone->text();

    if(p.length() == 13) {
        return;
    }

    QString t = b->text();

    switch(p.length()) {
    case 0:
        p += "(";
        break;

    case 4:
        p += ")";
        break;

    case 7:
    case 10:
        p += "-";
        break;
    }

    ui->lePhone->setText(p + t);
}

void TouchDlgPhoneNumber::on_btn0_clicked()
{
    buttonClicked(static_cast<QPushButton*>(sender()));
}

void TouchDlgPhoneNumber::on_btn1_clicked()
{
    buttonClicked(static_cast<QPushButton*>(sender()));
}

void TouchDlgPhoneNumber::on_btn2_clicked()
{
    buttonClicked(static_cast<QPushButton*>(sender()));
}

void TouchDlgPhoneNumber::on_btn3_clicked()
{
    buttonClicked(static_cast<QPushButton*>(sender()));
}

void TouchDlgPhoneNumber::on_btn4_clicked()
{
    buttonClicked(static_cast<QPushButton*>(sender()));
}

void TouchDlgPhoneNumber::on_btn5_clicked()
{
    buttonClicked(static_cast<QPushButton*>(sender()));
}

void TouchDlgPhoneNumber::on_btn6_clicked()
{
    buttonClicked(static_cast<QPushButton*>(sender()));
}

void TouchDlgPhoneNumber::on_btn7_clicked()
{
    buttonClicked(static_cast<QPushButton*>(sender()));
}

void TouchDlgPhoneNumber::on_btn8_clicked()
{
    buttonClicked(static_cast<QPushButton*>(sender()));
}

void TouchDlgPhoneNumber::on_btn9_clicked()
{
    buttonClicked(static_cast<QPushButton*>(sender()));
}

void TouchDlgPhoneNumber::on_btnCancel_clicked()
{
    reject();
}

void TouchDlgPhoneNumber::on_btnBackspace_clicked()
{
    QString t = ui->lePhone->text();
    t.remove(t.length() - 1, 1);
    QChar c = t.at(t.length() - 1);

    switch(c.toLatin1()) {
    case '(':
    case ')':
    case '-':
        t.remove(t.length() - 1, 1);
        break;
    }

    ui->lePhone->setText(t);
}
