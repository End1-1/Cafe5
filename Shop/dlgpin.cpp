#include "dlgpin.h"
#include "ui_dlgpin.h"

DlgPin::DlgPin(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::DlgPin)
{
    ui->setupUi(this);
}

DlgPin::~DlgPin()
{
    delete ui;
}

bool DlgPin::getPin(QString &pin, QString &pass)
{
    bool result = false;
    DlgPin *d = new DlgPin();
    if (d->exec() == QDialog::Accepted) {
        pin = d->ui->leUser->text();
        pass = d->ui->lePin->text();
        result = true;
    }
    delete d;
    return result;
}

void DlgPin::btnNumPressed()
{
    QPushButton *btn = static_cast<QPushButton*>(sender());
    QLineEdit *l = ui->leUser;
    if (ui->lePin->hasFocus()) {
        l = ui->lePin;
    }
    l->setText(l->text() + btn->text());
}

void DlgPin::on_leUser_textChanged(const QString &arg1)
{
    if (arg1.length() == 4) {
        ui->lePin->setFocus();
    }
}

void DlgPin::on_lePin_textChanged(const QString &arg1)
{
    if (arg1.length() == 4) {
        on_btnEnter_clicked();
    }
}

void DlgPin::on_btnEnter_clicked()
{
    if (ui->lePin->text().length() == 4 && ui->leUser->text().length() == 4) {
        accept();
    }
}

void DlgPin::on_btnClear_clicked()
{
    ui->lePin->clear();
    ui->leUser->clear();
    ui->leUser->setFocus();
}

void DlgPin::on_btnClose_clicked()
{
    reject();
}

void DlgPin::on_btn1_clicked()
{
    btnNumPressed();
}

void DlgPin::on_btn2_clicked()
{
    btnNumPressed();
}

void DlgPin::on_btn3_clicked()
{
    btnNumPressed();
}

void DlgPin::on_btn4_clicked()
{
    btnNumPressed();
}

void DlgPin::on_btn5_clicked()
{
    btnNumPressed();
}

void DlgPin::on_btn6_clicked()
{
    btnNumPressed();
}

void DlgPin::on_btn7_clicked()
{
    btnNumPressed();
}

void DlgPin::on_btn8_clicked()
{
    btnNumPressed();
}

void DlgPin::on_btn9_clicked()
{
    btnNumPressed();
}

void DlgPin::on_btn0_clicked()
{
    btnNumPressed();
}
