#include "touchentertaxreceiptnumber.h"
#include "ui_touchentertaxreceiptnumber.h"

TouchEnterTaxReceiptNumber::TouchEnterTaxReceiptNumber() :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::TouchEnterTaxReceiptNumber)
{
    ui->setupUi(this);
}

TouchEnterTaxReceiptNumber::~TouchEnterTaxReceiptNumber()
{
    delete ui;
}

bool TouchEnterTaxReceiptNumber::getTaxReceiptNumber(QString &number)
{
    TouchEnterTaxReceiptNumber d;
    if (d.exec() == QDialog::Accepted) {
        number = d.ui->leNumber->text();
        return true;
    }
    return false;
}

void TouchEnterTaxReceiptNumber::on_btnBackspace_clicked()
{
    QString t = ui->leNumber->text();
    t.remove(t.length() - 1, 1);
    ui->leNumber->setText(t);
}

void TouchEnterTaxReceiptNumber::on_btnOk_clicked()
{
    if (ui->leNumber->text().isEmpty() == false) {
        accept();
    }
}

void TouchEnterTaxReceiptNumber::on_btn1_clicked()
{
    buttonClicked(static_cast<QPushButton*>(sender()));
}

void TouchEnterTaxReceiptNumber::on_btn0_clicked()
{
    buttonClicked(static_cast<QPushButton*>(sender()));
}

void TouchEnterTaxReceiptNumber::on_btn2_clicked()
{
    buttonClicked(static_cast<QPushButton*>(sender()));
}

void TouchEnterTaxReceiptNumber::on_btn3_clicked()
{
    buttonClicked(static_cast<QPushButton*>(sender()));
}

void TouchEnterTaxReceiptNumber::on_btn4_clicked()
{
    buttonClicked(static_cast<QPushButton*>(sender()));
}

void TouchEnterTaxReceiptNumber::on_btn5_clicked()
{
    buttonClicked(static_cast<QPushButton*>(sender()));
}

void TouchEnterTaxReceiptNumber::on_btn6_clicked()
{
    buttonClicked(static_cast<QPushButton*>(sender()));
}

void TouchEnterTaxReceiptNumber::on_btn7_clicked()
{
    buttonClicked(static_cast<QPushButton*>(sender()));
}

void TouchEnterTaxReceiptNumber::on_btn8_clicked()
{
    buttonClicked(static_cast<QPushButton*>(sender()));
}

void TouchEnterTaxReceiptNumber::on_btn9_clicked()
{
    buttonClicked(static_cast<QPushButton*>(sender()));
}

void TouchEnterTaxReceiptNumber::buttonClicked(QPushButton *b)
{
    QString t = static_cast<QPushButton*>(b)->text();
    ui->leNumber->setText(ui->leNumber->text() + t);
}

void TouchEnterTaxReceiptNumber::on_btnCancel_clicked()
{
    reject();
}
